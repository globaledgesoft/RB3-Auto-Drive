#include "server.hpp"
#include "image_resize.hpp"

/************************************************************************
* name : argmax
* function: Returns the max index
************************************************************************/
template <typename T>
void argmax(std::vector<T>& data, int& max_id, T& max_pred) {
  max_id = 0;
  max_pred = 0;
  for (int i = 0; i < data.size(); i++) {
    if (data[i] > max_pred) {
      max_pred = data[i];
      max_id = i;
    }
  }
}

/************************************************************************
* name : RunInference
* function: Runs the dlr model
************************************************************************/
template <typename T>
void RunInference(DLRModelHandle model, std::vector<T> in_data,
                  const std::string& input_name,
                  std::vector<std::vector<T>>& outputs, const std::string& model_name) {
    
    int num_outputs;
    GetDLRNumOutputs(&model, &num_outputs);
    for (int i = 0; i < num_outputs; i++) {
        int64_t cur_size = 0;
        int cur_dim = 0;
        GetDLROutputSizeDim(&model, i, &cur_size, &cur_dim);
        std::vector<T> output(cur_size, 0);
        outputs.push_back(output);
    }

    std::vector<unsigned long> in_shape_ul;
    if (model_name == "class") {
        in_shape_ul = {1, 128, 128, 3};
    } else {
        in_shape_ul = {1, 256, 256, 3};
    }
    
    std::vector<int64_t> in_shape =
      std::vector<int64_t>(in_shape_ul.begin(), in_shape_ul.end());
    int64_t in_ndim = in_shape.size();

    if (SetDLRInput(&model, input_name.c_str(), in_shape.data(),
                    in_data.data(), static_cast<int>(in_ndim)) != 0) {
        throw std::runtime_error("Could not set input '" + input_name + "'");
    }
    if (RunDLRModel(&model) != 0) {
        throw std::runtime_error("Could not run");
    }
    for (int i = 0; i < num_outputs; i++) {
        if (GetDLROutput(&model, i, outputs[i].data()) != 0) {
            throw std::runtime_error("Could not get output" + std::to_string(i));
        }
    }
}

/************************************************************************
* name : sigchild_handler
* function: setting signal handler to avoid zombie processes
************************************************************************/
void sigchld_handler(int s)
{
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
}

/************************************************************************
* name : get_in_addr
* function: Returns ipV4 or ipV6 adddress based on family
************************************************************************/
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/************************************************************************
* name : main
* function: main function of server
************************************************************************/
int main() {

    int device_type = 1;
    std::string cls_input_name = CLASS_IP_NODE;
    std::string seg_input_name = SEG_IP_NODE;
    std::string cls_model_path = CLASS_MODEL_PATH;
    std::string seg_model_path = SEG_MODEL_PATH;
    std::vector<float> inp (DLR_SEG_H * DLR_SEG_W * DLR_SEG_C, 0);
    std::vector<float> inp_class (DLR_CLASS_H * DLR_CLASS_W * DLR_CLASS_C, 0);
   	std::vector<std::vector<float>> cls_outputs;
 	std::vector<std::vector<float>> seg_outputs;
    float no_obj = -1.0;
    int max_id;
    float max_pred;
    int sockfd, new_fd;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    int recv_bytes;
    int send_bytes;
    int total_recv = 0;
    int total_send = 0;

    std::cout << "Loading classification model... " << std::endl;
    DLRModelHandle cls_model = NULL;
    if (CreateDLRModel(&cls_model, cls_model_path.c_str(), device_type, 0) != 0) {
        throw std::runtime_error("Could not load DLR Model");
    } else {
        std::cout << "Loaded classification model " << std::endl;
    }

    std::cout << "Loading segmentation model... " << std::endl;
    DLRModelHandle seg_model = NULL;
    if (CreateDLRModel(&seg_model, seg_model_path.c_str(), device_type, 0) != 0) {
        throw std::runtime_error("Could not load DLR Model");
    } else {
        std::cout << "Loaded segmentation model " << std::endl;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }

    freeaddrinfo(servinfo);

    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections..\n");

    sin_size = sizeof their_addr;

    while ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) > 0)
    {
        inet_ntop(their_addr.ss_family,
                get_in_addr((struct sockaddr *)&their_addr),
                s, sizeof s);
        printf("server: got connection from %s\n", s);

        if (fork() == 0)
        {
            close(sockfd);
	        for(int i=0; i<10; i++) {
                total_recv = 0;
                total_send = 0;
            	while(total_recv < DLR_SEG_H * DLR_SEG_W * DLR_SEG_C * sizeof(float)) {
                    if ((recv_bytes = recv(new_fd,(uint8_t*)inp.data() + total_recv, NO_BYTES, 0)) < 0) {
                        perror("recv");
                        exit(1);
                    }
                    total_recv+=recv_bytes;
                }
            	max_id = -1;
            	max_pred = 0;
                inp_class = resize(inp, DLR_CLASS_W, DLR_CLASS_H);
            	RunInference(cls_model, inp_class, cls_input_name, cls_outputs, "class");
            	argmax(cls_outputs[0], max_id, max_pred);
                std::cout << "Max probability is " << max_pred << " at index " << max_id
                    << std::endl;
                if(max_id != 1) {
                	RunInference(seg_model, inp, seg_input_name, seg_outputs, "seg");
                    while(total_send < DLR_SEG_H * DLR_SEG_W * sizeof(float)) {
                        if ((send_bytes = send(new_fd, (uint8_t*)seg_outputs[0].data() + total_send, NO_BYTES, 0)) == -1) {
                            perror("send");
                            exit(1);
                        }
                        total_send+=send_bytes;
                    }
                    std::cout << "Segmented Image send to client " << std::endl;
                } else {
                    if (send(new_fd, &no_obj, sizeof(no_obj), 0) == -1) {
                        perror("send");
                        exit(1);
                    }
                    std::cout << "No object:: -1.0 send" << std::endl;
                }
	    }
            close(new_fd);
            exit(0);
        }
        close(new_fd);
    }
    return 0;
}
