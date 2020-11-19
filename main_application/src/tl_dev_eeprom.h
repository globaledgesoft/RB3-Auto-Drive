/********************************************************************/
/**
 * @file	tl_dev_eeprom.h
 * @brief	load EEPROM for device control
 * @copyright	ThunderSoft Corporation.
 */
/********************************************************************/

#ifndef H_TL_DEV_EEPROM
#define H_TL_DEV_EEPROM

/*--------------------------------------------------------------------
	include headers
--------------------------------------------------------------------*/
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
/*--------------------------------------------------------------------
	definitions
--------------------------------------------------------------------*/
#define TL_MODE_MAX 2
#define TL_AFE_READ_SIZE_OFFSET		(7U)	/* offset of read_size2 */
#define TL_AFE_START_V_OFFSET		(750U)	/* offset of start_v */
#define TL_AFE_ARY_SIZE(ary)		(sizeof((ary)) / sizeof((ary)[0]))
/*------------------------*/
/* Common area            */
/*------------------------*/
/* common : camera type information */
typedef struct {
	char        mod_name[32];       /* name of camera module */
	char        afe_name[32];       /* name of AFE */
	char        sns_name[32];       /* name of image sensor */
	char        lns_name[32];       /* name of lens */
} tl_dev_rom_cam_type;

/* common : camera information */
typedef struct {
	uint16_t    sno_l;              /* number of serial lower[15:0] */
	uint16_t    map_ver;            /* eeprom map version */
	uint16_t    sno_u;              /* number of serial upper[31:16] */
	uint16_t    ajust_date;         /* date of adjustment */
	uint16_t    ajust_no;           /* number of adjustment */
	uint16_t    mod_type1;          /* module type 1 */
	uint16_t    mod_type2;          /* module type 2 */
	uint16_t    afe_ptn_id;         /* AFE pattern ID */
	uint16_t    opt_axis_center_h;  /* optical axis center (horizonal) [pixel] */
	uint16_t    opt_axis_center_v;  /* optical axis center (vertical)  [pixel] */
	uint16_t    fov_h;              /* horizontal angle of FOV */
	uint16_t    fov_v;              /* vertical angle of FOV */
	uint16_t    pixel_pitch;        /* hundredfold of pixel pitch[um] */
	uint16_t    focal_len;          /* hundredfold of focal length[mm]  */
	uint16_t    enable_mode;        /* enable mode flag */
	uint16_t    tal_mode;           /* TAL mode flag */
	uint16_t    pup_size;           /* size of Power Up Sequence */
	uint16_t    check_sum;          /* check sum */
} tl_dev_rom_cam_info;

/* common : lens parameters */
typedef struct {
	int64_t     planer_prm[4];      /* planer parameters */
	int64_t     distortion_prm[4];  /* distortion parameters */
} tl_dev_rom_lens;

/* common : Depth shading information */
typedef struct {
	uint16_t    offset[192];        /* shading setting */
	uint16_t    shd;                /* shading setting */
	uint16_t    x0;                 /* shading setting */
	uint16_t    xpwr[4];            /* shading setting */
	uint16_t    y0;	                /* shading setting */
	uint16_t    ypwr[3];            /* shading setting */
} tl_dev_rom_shading;

/* common : dfct */
typedef struct {
	uint16_t    dfct_pix_th_tbl[12];/* dfct look up table value */
	uint16_t    dfct[2];            /* dfct setting */
} tl_dev_rom_dfct;

/* common : AFE timing & mipi setting */
typedef struct {
	uint16_t    shp_loc;            /* SHP edge locations */
	uint16_t    shd_loc;            /* SHD edge location */
	uint16_t    output;             /* AFE output setting */
	uint16_t    output_sel;         /* image output setting(default) */
	uint16_t    vc;                 /* MIPI virtual channel setting */
} tl_dev_rom_timing_mipi;

/* common : cut out */
typedef struct {
	uint16_t    grid3;              /* cut out */
} tl_dev_rom_cutout;

/* common : IR processing */
typedef struct {
	uint16_t    ir1;                /* IR gain */
	uint16_t    ir_gmm[3];          /* IR gamma */
	uint16_t    ir_gmm_y[9];        /* IR gamma */
} tl_dev_rom_ir;

/* common : houndstooth detection */
typedef struct {
	uint16_t    upprth;             /* houndstooth detection setting */
	uint16_t    lwrth;              /* houndstooth detection setting */
	uint16_t    start_v;            /* houndstooth detection setting */
	uint16_t    start_h;            /* houndstooth detection setting */
	uint16_t    size_h;             /* houndstooth detection setting */
	uint16_t    upprerr_h;          /* houndstooth detection setting */
	uint16_t    upprerr_v;          /* houndstooth detection setting */
	uint16_t    lwrerr_h;           /* houndstooth detection setting */
	uint16_t    lwrerr_v;           /* houndstooth detection setting */
	uint16_t    det_ena;            /* houndstooth detection setting */
} tl_dev_rom_chkr;


/* common area informations */
typedef struct {
	tl_dev_rom_cam_type     cam_type;       /* camera type information */
	tl_dev_rom_cam_info     cam_info;       /* camera information  */
	tl_dev_rom_lens         lens;           /* lens parameters */
	tl_dev_rom_shading      shading;        /* Depth shading information */
	tl_dev_rom_dfct         dfct;           /* dfct */
	tl_dev_rom_timing_mipi  timing_mipi;    /* AFE timing & mipi setting */
	tl_dev_rom_cutout       cutout;         /* cut out */
	tl_dev_rom_ir           ir;             /* IR processing */
	tl_dev_rom_chkr         chkr;           /* houndstooth detection */
} tl_dev_rom_common;



/*------------------------*/
/* Mode area              */
/*------------------------*/
/* mode : mode information */
typedef struct {
	uint16_t    tof_mode_flag;      /* TOF mode */
	uint16_t    ld_flag;            /* number of LD */
	uint16_t    range_near_limit;   /* near distance */
	uint16_t    range_far_limit;    /* far distance */
	uint16_t    depth_unit;         /* unit of depth */
	uint16_t    fps;                /* frame rate */
} tl_dev_rom_mode_info;

/* mode : exposure */
typedef struct {
	uint16_t    exp_max;            /* exposure max value */
	uint16_t    exp_default;        /* exposure default value */
} tl_dev_rom_exp;

/* mode : AE parameter */
typedef struct {
	uint16_t    distance;           /* distance */
	uint16_t    lumine_num;         /* number of lumine */
	uint16_t    slope;              /* slope correction */
	int16_t     zofst;              /* zero point offset */
} tl_dev_rom_ae;

/* mode : AFE address of exposure setting */
typedef struct {
	uint8_t     long_num;           /* number of long address */
	uint8_t     short_num;          /* number of short address */
	uint8_t     lms_num;            /* number of lms address */
	uint16_t    long_addr[15];      /* address of long  ... NDR:exposure  WDR:long  */
	uint16_t    short_addr[4];      /* address of short ... NDR:not used  WDR:short */
	uint16_t    lms_addr[4];        /* address of lms   ... NDR:not used  WDR:long-short-1 */
} tl_dev_rom_exp_addr;

/* mode : AFE address of dummy transfer */
typedef struct {
	uint16_t    addr_num;           /* number of address */
	uint16_t    addr[4];            /* address of dummy transfer */
} tl_dev_rom_ccd_addr;

/* mode : Parameters of exposure setting */
typedef struct {
	uint16_t    vd_ini_ofst;        /* HD number from VD to exposure start */
	uint16_t    vd_ini_ofst_adr_num;/* number of AFE address to set "vd_ini_ofst" */
	uint16_t    vd_init_ofst_adr[4];/* address to set "vd_ini_ofst"  */
	uint16_t    ld_pls_duty;        /* TOF pulse duty ratio */
	uint16_t    vd_duration;
	/* VD length (HD number within 1 VD) ** this value is orignal VD - 2  */
	uint16_t    vd_reg_adr;         /* addres to set "vd_duration" */
	uint16_t    num_clk_in_hd;      /* Number of clocks in 1 HD */
	uint16_t    beta_num;           /* Number of light emission and exposure */
	uint16_t    num_hd_in_readout;  /* Number of HDs in ReadOut */
	uint16_t    clk_width_u;        /* Reference clock(x1000000) : upper 2byte */
	uint16_t    clk_width_l;        /* Reference clock(x1000000) : lower 2byte */
	uint16_t    tof_emt_period_ofst;
	/* Number of HDs for adjusting the light emission exposure period */
	uint16_t    tof_seq_ini_ofst;
	/* Offset from the positive edge of HD to the start of exposure */
	uint16_t	idle_peri_num;		/* Number of idle*/
	uint16_t	idle_peri_adr[4];	/* Each add of idle*/
	uint16_t    afe_idle_val[4];	/* afe value*/
} tl_dev_rom_exp_prm;

/* mode : nonlinear correction */
typedef struct {
	uint16_t    offset[49];         /* nonlinear offset */
	uint16_t    x0;                 /* nonlinear setting */
	uint16_t    xpwr[12];           /* nonlinear setting */
} tl_dev_rom_nlr;

/* mode : depth correction */
typedef struct {
	uint16_t    depth0;              /* zero point offset */
	uint16_t    depth2;              /* slope correction of depth */
	uint16_t    depth3;              /* slope correction of depth */
} tl_dev_rom_depth;

/* mode : AFE timing control */
typedef struct {
	uint16_t    rate_adjust[3];      /* rate adjust */
	uint16_t    align[11];           /* align */
	uint16_t    read_size[7];        /* read size */
	uint16_t    roi[8];              /* ROI */
} tl_dev_rom_mode_timing;

/* mode : grid conversion */
typedef struct {
	uint16_t    grid[3];             /* grid conversion */
} tl_dev_rom_grid;

/* mode : RAW noise reduction */
typedef struct {
	uint16_t    xpwr[3];             /* Raw-NR setting */
	uint16_t    bl_tbl[13];          /* Raw-NR setting */
	uint16_t    med;                 /* Raw-NR setting */
	uint16_t    sat_th;              /* Raw-NR setting */
	uint16_t    bk_tbl[13];          /* Raw-NR setting */
} tl_dev_rom_raw_nr;

/* mode : coring */
typedef struct {
	uint16_t    cor[3];              /* coring setting */
	uint16_t    corb[3];             /* coring setting */
	uint16_t    corf[3];             /* coring setting */
} tl_dev_rom_coring;

/* mode : depth control */
typedef struct {
	uint16_t    depth1;              /* method of TOF */
	uint16_t    control;             /* contorl of depth */
} tl_dev_rom_depth_ctrl;

/* mode : Duty modulation of TOF pulse */
typedef struct {
	uint16_t    control;             /* contorl of TOF pluse */
	uint16_t    val[10];             /* setting value */
} tl_dev_rom_pls_mod;

/* mode : Duty modulation of TOF pulse */
typedef struct {
    uint16_t           afe_revision;       /* afe_revision:0xC0FF*/
} tl_dev_afe;
/* mode area information */
typedef struct {
	tl_dev_rom_mode_info    info;               /* mode information */
	tl_dev_rom_exp          exp;                /* exposure */
	tl_dev_rom_ae           ae[3];              /* AE parameters */
	tl_dev_rom_exp_addr     exp_addr;           /* AFE address of exposure */
	tl_dev_rom_ccd_addr     ccd_addr;           /* AFE address of dummy transfer */
	tl_dev_rom_exp_prm      exp_prm;            /* Parameters of exposure setting */
	tl_dev_rom_nlr          nlr;                /* nonlinear correction */
	tl_dev_rom_depth        depth;              /* depth correction */
	tl_dev_rom_mode_timing  timing;             /* AFE timing control */
	tl_dev_rom_grid         grid;               /* grid conversion */
	tl_dev_rom_raw_nr       raw_nr;             /* RAW noise reduction */
	tl_dev_rom_coring       coring;             /* coring */
	tl_dev_rom_depth_ctrl   depth_ctrl;         /* depth control */
	tl_dev_rom_pls_mod      pls_mod;            /* Duty modulation of TOF pulse */
} tl_dev_rom_mode;



/*------------------------*/
/* ALL                    */
/*------------------------*/
/* EEPROM data for device control */
typedef struct {
	tl_dev_rom_common   cmn;                /* common TOF device info */
	tl_dev_rom_mode     mode[TL_MODE_MAX];  /* data of each mode */
	tl_dev_afe          afe_val;           	/* afe value */
} tl_dev_eeprom;

typedef struct {
	uint16_t	long_val;		/* number of lumine(long) */
	uint16_t	short_val;		/* number of lumine(short) */
	uint16_t	lms_val;		/* number of lumine(lms) */
	uint16_t	read_size2;		/* when to start output */
	uint16_t	ccd_dummy;		/* ccd dummy transfer */
	uint16_t	chkr_start_v;		/* Vsync timing */
	uint16_t    	idle;			/* idle */
	uint16_t 	ini_ofst;		/* ini ofst */
} tl_afe_exp_val;

typedef struct {
	int   			mode;			/* Ranging mode */
	tl_afe_exp_val      	p_exp;			/* afe exposure value */
	int     		image_type_output_sel;	/* image output setting(default) */
	int			external_sync;		/* tl synchronization*/
} tl_transmit_kernel;

typedef struct {
	uint32_t 		exp_hd;
	uint16_t    		idle_delay;
	/* The number of HDs in Idle (in/out) */
	uint16_t    		ini_ofst_delay;
	/* The number of HDs in Initial offset (in/out) */
	tl_transmit_kernel	tl_transmit;
	/* Communicate with the kernel*/
} tl_temp_val;


#endif /* H_TL_DEV_EEPROM */
