/*****************************************************************************
 * RRDtool 1.0.48  Copyright Tobias Oetiker, 1997, 1998, 1999
 *****************************************************************************
 * rrd_format.h  RRD Database Format header
 *****************************************************************************/

#define printf php_printf
#define puts php_printf
#define free efree
#define malloc emalloc

#define RRD_READONLY	0
#define RRD_READWRITE	1

typedef union unival {
	unsigned long	u_cnt;
	rrd_value_t		u_val;
} unival;

/****************************************************************************
 * POS 1: stat_head_t                           static header of the database
 ****************************************************************************/

typedef struct stat_head_t {
	/* Data Base Identification Section ***/
	char			cookie[4];
	char			version[5];
	double			float_cookie;

	/* Data Base Structure Definition *****/
	unsigned long	ds_cnt;
	unsigned long	rra_cnt;
	unsigned long	pdp_step;
	unival			par[10];
} stat_head_t;

/****************************************************************************
 * POS 2: ds_def_t  (* ds_cnt)                        Data Source definitions
 ****************************************************************************/
enum ds_param_en {
	DS_mrhb_cnt = 0,
	DS_min_val,
	DS_max_val
};

/* The magic number here is one less than DS_NAM_SIZE */
#define DS_NAM_SIZE	20
#define DST_SIZE	20

typedef struct ds_def_t {
	char			ds_nam[DS_NAM_SIZE];
	char			dst[DST_SIZE];
	unival			par[10];
} ds_def_t;

/****************************************************************************
 * POS 3: rra_def_t ( *  rra_cnt)         one for each store to be maintained
 ****************************************************************************/
enum rra_par_en {
	RRA_cdp_xff_val =0
};

#define CF_NAM_SIZE	20

typedef struct rra_def_t {
	char			cf_nam[CF_NAM_SIZE];
	unsigned long	row_cnt;
	unsigned long	pdp_cnt;
	unival			par[10];
} rra_def_t;

/****************************************************************************
 * POS 4: live_head_t                    
 ****************************************************************************/

typedef struct live_head_t {
	time_t			last_up;
} live_head_t;

/****************************************************************************
 * POS 5: pdp_prep_t  (* ds_cnt)                     here we prepare the pdps 
 ****************************************************************************/
#define LAST_DS_LEN 30 /* DO NOT CHANGE THIS ... */

enum pdp_par_en {
	PDP_unkn_sec_cnt = 0,
	PDP_val
};

typedef struct pdp_prep_t {
	char last_ds[LAST_DS_LEN];
	unival			scratch[10];
} pdp_prep_t;

/****************************************************************************
 * POS 6: cdp_prep_t (* rra_cnt * ds_cnt )      data prep area for cdp values
 ****************************************************************************/
enum cdp_par_en {
	CDP_val=0,
	CDP_unkn_pdp_cnt
};

typedef struct cdp_prep_t {
	unival			scratch[10];
} cdp_prep_t;

/****************************************************************************
 * POS 7: rra_ptr_t (* rra_cnt)       pointers to the current row in each rra
 ****************************************************************************/

typedef struct rra_ptr_t {
	unsigned long	cur_row;
} rra_ptr_t;

/****************************************************************************
 ****************************************************************************
 * One single struct to hold all the others. For convenience.
 ****************************************************************************
 ****************************************************************************/
typedef struct rrd_t {
	stat_head_t		*stat_head;
	ds_def_t		*ds_def;
	rra_def_t		*rra_def;
	live_head_t		*live_head;
	pdp_prep_t		*pdp_prep;
	cdp_prep_t		*cdp_prep;
	rra_ptr_t		*rra_ptr;
	rrd_value_t		*rrd_value;
} rrd_t;

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
