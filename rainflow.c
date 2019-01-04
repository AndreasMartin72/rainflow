/*
 *   |     .-.
 *   |    /   \         .-.
 *   |   /     \       /   \       .-.     .-.     _   _
 *   +--/-------\-----/-----\-----/---\---/---\---/-\-/-\/\/---
 *   | /         \   /       \   /     '-'     '-'
 *   |/           '-'         '-'
 *
 *          ____  ___    _____   __________    ____ _       __
 *         / __ \/   |  /  _/ | / / ____/ /   / __ \ |     / /
 *        / /_/ / /| |  / //  |/ / /_  / /   / / / / | /| / / 
 *       / _, _/ ___ |_/ // /|  / __/ / /___/ /_/ /| |/ |/ /  
 *      /_/ |_/_/  |_/___/_/ |_/_/   /_____/\____/ |__/|__/   
 *
 *    Rainflow Counting Algorithm (4-point-method), C99 compliant
 * 
 * 
 * "Rainflow Counting" consists of four main steps:
 *    1. Hysteresis Filtering
 *    2. Peak-Valley Filtering
 *    3. Discretization
 *    4. Four Point Counting Method:
 *    
 *                     * D
 *                    / \       Closed, if min(B,C) >= min(A,D) && max(B,C) <= max(A,D)
 *             B *<--/          Slope B-C is counted and removed from residue
 *              / \ /
 *             /   * C
 *          \ /
 *           * A
 *
 * These steps are fully documented in standards such as 
 * ASTM E1049 "Standard Practices for Cycle Counting in Fatigue Analysis" [1].
 * This implementation uses the 4-point algorithm mentioned in [3,4] and the 3-point HCM method proposed in [2].
 * To take the residue into account, you may implement a custom method or use some
 * predefined functions.
 * 
 * References:
 * [1] "Standard Practices for Cycle Counting in Fatigue Analysis."
 *     ASTM Standard E 1049, 1985 (2011). 
 *     West Conshohocken, PA: ASTM International, 2011.
 * [2] "Rainflow - HCM / Ein Hysteresisschleifen-Zaehlalgorithmus auf werkstoffmechanischer Grundlage"
 *     U.H. Clormann, T. Seeger
 *     1985 TU Darmstadt, Fachgebiet Werkstoffmechanik
 * [3] "Zaehlverfahren zur Bildung von Kollektiven und Matrizen aus Zeitfunktionen"
 *     FVA-Richtlinie, 2010.
 *     [https://fva-net.de/fileadmin/content/Richtlinien/FVA-Richtlinie_Zaehlverfahren_2010.pdf]
 * [4] Siemens Product Lifecycle Management Software Inc., 2018. 
 *     [https://community.plm.automation.siemens.com/t5/Testing-Knowledge-Base/Rainflow-Counting/ta-p/383093]
 * [5] "Review and application of Rainflow residue processing techniques for accurate fatigue damage estimation"
 *     G.Marsh;
 *     International Journal of Fatigue 82 (2016) 757-765,
 *     [https://doi.org/10.1016/j.ijfatigue.2015.10.007]
 * [6] "Betriebsfestigkeit - Verfahren und Daten zur Bauteilberechnung"
 *     Haibach, Erwin; Springer Verlag
 * []  "Schaedigungsbasierte Hysteresefilter"; Hack, M, D386 (Diss Univ. Kaiserslautern), Shaker Verlag Aachen, 1998, ISBN 3-8265-3936-2
 * []  "Hysteresis and Phase Transition"
 *     Brokate, M.; Sprekels, J.; Applied Mathematical Sciences 121; Springer, New York, 1996
 * []  "Rainflow counting and energy dissipation in elastoplasticity"; Eur. J. Mech. A/Solids 15, pp. 705-737, 1996
 *     Brokate, M.; Dressler, K.; Krejci, P.
 * []  "Multivariate Density Estimation: Theory, Practice and Visualization". New York, Chichester, Wiley & Sons, 1992
 *     Scott, D.
 * []  "Werkstoffmechanik - Bauteile sicher beurteilen undWerkstoffe richtig einsetzen"; 
 *      Ralf B?rgel, Hans Albert Richard, Andre Riemer; Springer FachmedienWiesbaden 2005, 2014
 * []  "Zaelverfahren und Lastannahme in der Betriebsfestigkeit";
 *     Michael Koehler, Sven Jenne / Kurt Poetter, Harald Zenner; Springer-Verlag Berlin Heidelberg 2012
 *
 *                                                                                                                                                          *
 *================================================================================
 * BSD 2-Clause License
 * 
 * Copyright (c) 2018, Andras Martin
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *================================================================================
 */

/* This version is generated via coan (http://coan2.sourceforge.net/) */


#include "rainflow.h"

#include <assert.h>  /* assert() */
#include <math.h>    /* exp(), log(), fabs() */
#include <stdlib.h>  /* calloc(), free(), abs() */
#include <string.h>  /* memset() */


#ifdef MATLAB_MEX_FILE
#define RFC_MEX_USAGE \
"\nUsage:\n"\
"[pd,re,rm] = rfc( data, class_count, class_width, class_offset, hysteresis )\n"\
"    pd = Pseudo damage\n"\
"    re = Residue\n"\
"    rm = Rainflow matrix (from/to)\n"
#pragma message(RFC_MEX_USAGE)
#include <ctype.h>
#include <string.h>
#include <mex.h>
#endif /*MATLAB_MEX_FILE*/

/* Core functions */
#define RFC_cycle_find      RFC_cycle_find_4ptm
static void                 RFC_wl_init                         ( rfc_ctx_s *, double sd, double nd, double k );
static bool                 RFC_feed_once                       ( rfc_ctx_s *, const rfc_value_tuple_s* tp, int flags );
static bool                 RFC_feed_finalize                   ( rfc_ctx_s * );
static rfc_value_tuple_s *  RFC_feed_filter_pt                  ( rfc_ctx_s *, const rfc_value_tuple_s *pt );
static void                 RFC_cycle_find_4ptm                 ( rfc_ctx_s *, int flags );
static void                 RFC_cycle_process_counts            ( rfc_ctx_s *, rfc_value_tuple_s *from, rfc_value_tuple_s *to, rfc_value_tuple_s *next, int flags );
/* Methods on residue */
static bool                 RFC_finalize_res_ignore             ( rfc_ctx_s *, int flags );
static void                 RFC_residue_remove_item             ( rfc_ctx_s *, size_t index, size_t count );
/* Memory allocator */
static void *               RFC_mem_alloc                       ( void *ptr, size_t num, size_t size, int aim );
/* Other */
static double               RFC_damage_calc_amplitude           ( rfc_ctx_s *, double amplitude );
static double               RFC_damage_calc                     ( rfc_ctx_s *, unsigned class_from, unsigned class_to, double *Sa_ret );
static bool                 RFC_error_raise                     ( rfc_ctx_s *, int );
static RFC_value_type       value_delta                         ( RFC_value_type from_val, RFC_value_type to, int *sign_ptr );


#define QUANTIZE( r, v )    ( (unsigned)( ((v) - (r)->class_offset) / (r)->class_width ) )
#define CLASS_MEAN( r, c )  ( (double)( (r)->class_width * (0.5 + (c)) + (r)->class_offset ) )
#define CLASS_UPPER( r, c ) ( (double)( (r)->class_width * (1.0 + (c)) + (r)->class_offset ) )
#define NUMEL( x )          ( sizeof(x) / sizeof(*(x)) )


/**
 * @brief      Initialization (rainflow context).
 *
 * @param      ctx           The rainflow context
 * @param[in]  class_count   The class count
 * @param[in]  class_width   The class width
 * @param[in]  class_offset  The class offset
 * @param[in]  hysteresis    The hysteresis
#if RFC_TP_SUPPORT 
 * @param[in]  tp            Pointer to turning points buffer
 * @param[in]  tp_cap        Number of turning points in buffer
#endif
 *
 * @return     true on success
 */
bool RFC_init( void *ctx, unsigned class_count, RFC_value_type class_width, RFC_value_type class_offset, 
                          RFC_value_type hysteresis, int flags )
{
    rfc_ctx_s         *rfc_ctx = (rfc_ctx_s*)ctx;
    rfc_value_tuple_s  nil     = { 0.0 };  /* All other members are zero-initialized, see ISO/IEC 9899:TC3, 6.7.8 (21) */

    if( !rfc_ctx || rfc_ctx->version != sizeof(rfc_ctx_s) )
    {
        assert( false );
        rfc_ctx->error = RFC_ERROR_INVARG;
        return false;
    }

    if( rfc_ctx->state != RFC_STATE_INIT0 )
    {
        return false;
    }

    /* Flags */
    if( flags == RFC_FLAGS_DEFAULT )
    {
        flags                               = RFC_FLAGS_COUNT_RFM            | 
                                              RFC_FLAGS_COUNT_DAMAGE;
    }
    rfc_ctx->internal.flags                 = flags;

    /* Counter increments */
    rfc_ctx->full_inc                       = RFC_FULL_CYCLE_INCREMENT;
    rfc_ctx->half_inc                       = RFC_HALF_CYCLE_INCREMENT;
    rfc_ctx->curr_inc                       = RFC_FULL_CYCLE_INCREMENT;

    if( class_count )
    {
        if( class_count > 512 || class_width <= 0.0 )
        {
            rfc_ctx->error = RFC_ERROR_INVARG;
            return false;
        }
    }

    /* Rainflow class parameters */
    rfc_ctx->class_count                    = class_count;
    rfc_ctx->class_width                    = class_width;
    rfc_ctx->class_offset                   = class_offset;
    rfc_ctx->hysteresis                     = hysteresis;

    /* Values for a "pseudo Woehler curve" */
    RFC_wl_init( rfc_ctx, 1e3 /*sd*/, 1e7 /*nd*/, -5.0 /*k*/ );

    /* Memory allocator */
    if( !rfc_ctx->mem_alloc )
    {
        rfc_ctx->mem_alloc = RFC_mem_alloc;
    }

    /* Residue */
    rfc_ctx->internal.residue_cap           = NUMEL( rfc_ctx->internal.residue );
    rfc_ctx->residue_cnt                    = 0;
    rfc_ctx->residue_cap                    = 2 * rfc_ctx->class_count; /* max size is 2*n-1 plus interim point = 2*n */

    if( rfc_ctx->residue_cap <= rfc_ctx->internal.residue_cap )
    {
        rfc_ctx->residue_cap                = rfc_ctx->internal.residue_cap; /* At least 3 elements are needed (two to define a slope and one as interim point) */
        rfc_ctx->residue                    = rfc_ctx->internal.residue;
        rfc_ctx->internal.res_static        = true;
    }
    else
    {
        rfc_ctx->residue                    = (rfc_value_tuple_s*)rfc_ctx->mem_alloc( NULL, rfc_ctx->residue_cap, 
                                                                                      sizeof(rfc_value_tuple_s), RFC_MEM_AIM_RESIDUE );
        rfc_ctx->internal.res_static        = false;
    }

    if( rfc_ctx->class_count )
    {
        int ok = rfc_ctx->residue != NULL;

        if( ok && ( flags & RFC_FLAGS_COUNT_RFM ) )
        {
            /* Non-sparse storages (optional, may be NULL) */
            rfc_ctx->rfm                    = (RFC_counts_type*)rfc_ctx->mem_alloc( NULL, class_count * class_count, 
                                                                                    sizeof(RFC_counts_type), RFC_MEM_AIM_MATRIX );
            if( !rfc_ctx->rfm ) ok = false;
        }
        if( !ok )
        {
            RFC_deinit( rfc_ctx );
            rfc_ctx->error = RFC_ERROR_INVARG;

            return false;
        }
    }

    /* Damage */
    rfc_ctx->pseudo_damage                  = 0.0;

    /* Internals */
    rfc_ctx->internal.slope                 = 0;
    rfc_ctx->internal.extrema[0]            = nil;  /* local minimum */
    rfc_ctx->internal.extrema[1]            = nil;  /* local maximum */


    rfc_ctx->state = RFC_STATE_INIT;
    return true;
}


/**
 * @brief      De-initialization (freeing memory).
 *
 * @param      ctx   The rainflow context
 *
 * @return     true on success
 */
bool RFC_deinit( void *ctx )
{
    rfc_ctx_s         *rfc_ctx = (rfc_ctx_s*)ctx;
    rfc_value_tuple_s  nil     = { 0.0 };  /* All other members are zero-initialized, see ISO/IEC 9899:TC3, 6.7.8 (21) */

    if( !rfc_ctx || rfc_ctx->version != sizeof(rfc_ctx_s) )
    {
        assert( false );
        rfc_ctx->error = RFC_ERROR_INVARG;

        return false;
    }

    if( rfc_ctx->state < RFC_STATE_INIT )
    {
        return false;
    }

    if( !rfc_ctx->internal.res_static &&
        rfc_ctx->residue )              rfc_ctx->mem_alloc( rfc_ctx->residue,    0, 0, RFC_MEM_AIM_RESIDUE );
    if( rfc_ctx->rfm )                  rfc_ctx->mem_alloc( rfc_ctx->rfm,        0, 0, RFC_MEM_AIM_MATRIX );

    rfc_ctx->residue                    = NULL;
    rfc_ctx->residue_cap                = 0;
    rfc_ctx->residue_cnt                = 0;

    rfc_ctx->rfm                        = NULL;
    
    rfc_ctx->internal.slope             = 0;
    rfc_ctx->internal.extrema[0]        = nil;  /* local minimum */
    rfc_ctx->internal.extrema[1]        = nil;  /* local maximum */
    rfc_ctx->internal.pos               = 0;
    rfc_ctx->internal.global_offset     = 0;


    rfc_ctx->state = RFC_STATE_INIT0;

    return true;
}


/**
 * @brief      "Feed" counting algorithm with data samples (consecutive calls
 *             allowed).
 *
 * @param      ctx         The rainflow context
 * @param[in]  data        The data
 * @param[in]  data_count  The data count
 *
 * @return     true on success
 */
bool RFC_feed( void *ctx, const RFC_value_type * data, size_t data_count )
{
    rfc_ctx_s *rfc_ctx = (rfc_ctx_s*)ctx;

    if( data_count && !data ) return false;

    if( !rfc_ctx || rfc_ctx->version != sizeof(rfc_ctx_s) )
    {
        assert( false );
        rfc_ctx->error = RFC_ERROR_INVARG;

        return false;
    }

    if( rfc_ctx->state < RFC_STATE_INIT || rfc_ctx->state >= RFC_STATE_FINISHED )
    {
        return false;
    }

    /* Process data */
    while( data_count-- )
    {
        rfc_value_tuple_s tp = { *data++ };  /* All other members are zero-initialized, see ISO/IEC 9899:TC3, 6.7.8 (21) */

        /* Assign class and global position (base 1) */
        tp.cls = QUANTIZE( rfc_ctx, tp.value );
        tp.pos   = ++rfc_ctx->internal.pos;

        if( !RFC_feed_once( rfc_ctx, &tp, rfc_ctx->internal.flags ) ) return false;
    }

    return true;
}


/**
 * @brief      Finalize pending counts and turning point storage.
 *
 * @param      ctx              The rainflow context
 * @param[in]  residual_method  The residual method (RFC_RES_...)
 *
 * @return     true on success
 */
bool RFC_finalize( void *ctx, int residual_method )
{
    rfc_ctx_s *rfc_ctx = (rfc_ctx_s*)ctx;
    bool ok;

    if( !rfc_ctx || rfc_ctx->version != sizeof(rfc_ctx_s) )
    {
        assert( false );
        rfc_ctx->error = RFC_ERROR_INVARG;

        return false;
    }
    
    if( rfc_ctx->state < RFC_STATE_INIT || rfc_ctx->state >= RFC_STATE_FINISHED )
    {
        return false;
    }

    {
        int flags = rfc_ctx->internal.flags;


        switch( residual_method )
        {
            case RFC_RES_NONE:
                /* FALLTHROUGH */
            case RFC_RES_IGNORE:
                ok = RFC_finalize_res_ignore( rfc_ctx, flags );
                break;
            default:
                assert( false );
                rfc_ctx->error = RFC_ERROR_INVARG;
                ok = false;
        }
        assert( rfc_ctx->state == RFC_STATE_FINALIZE );
    }

    if( !rfc_ctx->class_count )
    {
        rfc_ctx->residue_cnt = 0;
    }

    rfc_ctx->state = ok ? RFC_STATE_FINISHED : RFC_STATE_ERROR;
    return ok;
}







/*** Implementation static functions ***/

/**
 * @brief      Initialize Woehler parameters
 *
 * @param      rfc_ctx  The rfc context
 * @param[in]  sd       Amplitude "SD"
 * @param[in]  nd       Cycles "ND"
 * @param[in]  k        Slope "k"
 */
static
void RFC_wl_init( rfc_ctx_s *rfc_ctx, double sd, double nd, double k )
{
    assert( rfc_ctx );

    /* Woehler curve (fictive) */
    rfc_ctx->wl_sd                          =  sd;
    rfc_ctx->wl_nd                          =  nd;
    rfc_ctx->wl_k                           = -fabs(k);
}


/**
 * @brief      Processing one data point. Find turning points and check for
 *             closed cycles.
 *
 * @param      rfc_ctx  The rainflow context
 * @param[in]  pt       The data tuple
 * @param[in]  flags    The flags
 *
 * @return     true on success
 */
static
bool RFC_feed_once( rfc_ctx_s *rfc_ctx, const rfc_value_tuple_s* pt, int flags )
{
    rfc_value_tuple_s *tp_residue;  /* Pointer to residue element */

    assert( rfc_ctx && pt );
    assert( rfc_ctx->state >= RFC_STATE_INIT && rfc_ctx->state < RFC_STATE_FINISHED );

    /* Check for next turning point and update residue. tp_residue is NULL, if there is no turning point */
    /* Otherwise tp_residue refers the forelast element in member rfc_ctx->residue */
    tp_residue = RFC_feed_filter_pt( rfc_ctx, pt );

    /* Countings */

    /* Add turning point and check for closed cycles */
    if( tp_residue )
    {
        if( rfc_ctx->class_count )
        {

            /* Check for closed cycles and count. Modifies residue! */
            RFC_cycle_find( rfc_ctx, flags );
        }
        else
        {
            if( rfc_ctx->residue_cnt > 1 )
            {
                RFC_residue_remove_item( rfc_ctx, 0, 1 );
            }
        }
    }

    return true;
}


/**
 * @brief      Handling interim turning point and margin. If there are still
 *             unhandled turning point left, "finalizing" takes this into
 *             account for the rainflow algorithm.
 *
 * @param      rfc_ctx  The rainflow context
 *
 * @return     true on success
 */
static
bool RFC_feed_finalize( rfc_ctx_s *rfc_ctx )
{
    rfc_value_tuple_s *tp_interim = NULL;

    assert( rfc_ctx );
    assert( rfc_ctx->state >= RFC_STATE_INIT && rfc_ctx->state < RFC_STATE_FINISHED );

    if( rfc_ctx->state < RFC_STATE_FINALIZE )
    {
        /* Adjust residue: Incorporate interim turning point */
        if( rfc_ctx->state == RFC_STATE_BUSY_INTERIM )
        {
            tp_interim = &rfc_ctx->residue[rfc_ctx->residue_cnt];
            rfc_ctx->residue_cnt++;

            rfc_ctx->state = RFC_STATE_BUSY;
        }


        if( tp_interim )
        {
            int flags = rfc_ctx->internal.flags;

            /* Check once more if a new cycle is closed now */
            RFC_cycle_find( rfc_ctx, flags );
        }


        rfc_ctx->state = RFC_STATE_FINALIZE;
    }

    return true;
}


/**
 * @brief      Finalize pending counts, ignore residue.
 *
 * @param      rfc_ctx  The rainflow context
 * @param[in]  flags    The flags
 *
 * @return     true on success
 */
static
bool RFC_finalize_res_ignore( rfc_ctx_s *rfc_ctx, int flags )
{
    assert( rfc_ctx );
    assert( rfc_ctx->state >= RFC_STATE_INIT && rfc_ctx->state < RFC_STATE_FINISHED );

    /* Include interim turning point */
    return RFC_feed_finalize( rfc_ctx );
}


/**
 * @brief      Remove items (points) from the residue
 *
 * @param      rfc_ctx  The rainflow context
 * @param[in]  index    The item position in residue, base 0
 * @param[in]  count    The number of elements to remove
 */
static
void RFC_residue_remove_item( rfc_ctx_s *rfc_ctx, size_t index, size_t count )
{
    size_t  from = index + count,
            to   = index, 
            end;

    assert( rfc_ctx );
    assert( rfc_ctx->state >= RFC_STATE_INIT && rfc_ctx->state < RFC_STATE_FINISHED );
    assert( rfc_ctx->residue && index + count <= rfc_ctx->residue_cnt );

    end = (int)rfc_ctx->residue_cnt;

    /* Example
                         |cnt(5)
                         v
               |O|O|X|O|O|
                   ^
                   |index(2)

        elements = 2    (5-2-1+0 = 2, no interim turning point)
    */

    if( rfc_ctx->state == RFC_STATE_BUSY_INTERIM )
    {
        /* Include interim turning point */
        end++;
    }

    /* Shift points */
    while( from < end )
    {
        rfc_ctx->residue[to++] = rfc_ctx->residue[from++];
    }

    rfc_ctx->residue_cnt -= count;
}


/**
 * @brief      Calculate pseudo damage for one cycle with given amplitude Sa
 *
 * @param      rfc_ctx  The rainflow context
 * @param[in]  Sa       The amplitude
 *
 * @return     Pseudo damage
 */
static
double RFC_damage_calc_amplitude( rfc_ctx_s *rfc_ctx, double Sa )
{
    assert( rfc_ctx );
    assert( rfc_ctx->state >= RFC_STATE_INIT );

    do {
        /* Constants for the Woehler curve */
        const double SD_log = log(rfc_ctx->wl_sd);
        const double ND_log = log(rfc_ctx->wl_nd);
        const double k      = rfc_ctx->wl_k;

        /* Pseudo damage */
        double D = 0.0;

        if( Sa >= 0.0 )
        {
            /* D =           h /    ND   *    ( Sa /    SD)  ^ ABS(k)   */
            /* D = exp(  log(h /    ND)  + log( Sa /    SD)  * ABS(k) ) */
            /* D = exp( (log(h)-log(ND)) + (log(Sa)-log(SD)) * ABS(k) ) */
            /* D = exp(      0 -log(ND)  + (log(Sa)-log(SD)) * ABS(k) ) */
            D = exp( fabs(k)  * ( log(Sa) - SD_log ) - ND_log );
        }
        else
        {
            assert( false );
        }

        return D;
        
    } while(0);
}


/**
 * @brief      Calculate fictive damage for one closed (full) cycle.
 *
 * @param      rfc_ctx     The rainflow context
 * @param[in]  class_from  The starting class
 * @param[in]  class_to    The ending class
 * @param[out] Sa_ret      The amplitude, may be NULL
 *
 * @return     Pseudo damage value for the closed cycle
 */
static
double RFC_damage_calc( rfc_ctx_s *rfc_ctx, unsigned class_from, unsigned class_to, double *Sa_ret )
{
    double damage    =  0.0;
    double amplitude = -1.0;  /* Negative amplitude indicates unset value */

    assert( rfc_ctx );
    assert( rfc_ctx->state >= RFC_STATE_INIT );

    if( class_from != class_to )
    {
        double range;

        range     = (double)rfc_ctx->class_width * abs( (int)class_to - (int)class_from );
        amplitude = range / 2.0;
        damage    = RFC_damage_calc_amplitude( rfc_ctx, amplitude );
    }

    if( Sa_ret )
    {
        *Sa_ret = amplitude;
    }

    return damage;
}


/**
 * @brief      Test data sample for a new turning point and add to the residue
 *             in that case. Update extrema.
 *             - 1. Hysteresis Filtering
 *             - 2. Peak-Valley Filtering
 *
 * @param      rfc_ctx  The rainflow context
 * @param[in]  pt       The data point, must not be NULL
 *
 * @return     Returns pointer to new turning point in residue or NULL
 */
static
rfc_value_tuple_s * RFC_feed_filter_pt( rfc_ctx_s *rfc_ctx, const rfc_value_tuple_s *pt )
{
    int                 slope;
    RFC_value_type      delta;
    rfc_value_tuple_s  *new_tp    = NULL;
    int                 do_append = 0;

    assert( rfc_ctx );
    assert( rfc_ctx->state >= RFC_STATE_INIT && rfc_ctx->state <= RFC_STATE_BUSY_INTERIM );

    if( !pt ) return NULL;

    slope = rfc_ctx->internal.slope;

    /* Handle first turning point(s) */
    if( rfc_ctx->state < RFC_STATE_BUSY_INTERIM )
    {
        /* Residue is empty, still searching first turning point(s) */

        if( rfc_ctx->state == RFC_STATE_INIT )
        {
            /* Very first point, initialize local min-max search */
            rfc_ctx->internal.extrema[0] = 
            rfc_ctx->internal.extrema[1] = *pt;
            rfc_ctx->state               =  RFC_STATE_BUSY;
        }
        else
        {
            int is_falling_slope = -1;

            assert( rfc_ctx->state == RFC_STATE_BUSY );

            /* Still searching for first turning point(s) */

            /* Update local extrema */
            if( pt->value < rfc_ctx->internal.extrema[0].value )
            {
                /* Minimum */
                is_falling_slope = 1;
                rfc_ctx->internal.extrema[0] = *pt;
            }
            else if( pt->value > rfc_ctx->internal.extrema[1].value )
            {
                /* Maximum */
                is_falling_slope = 0;
                rfc_ctx->internal.extrema[1] = *pt;
            }

            /* Local hysteresis filtering */
            delta = value_delta( rfc_ctx->internal.extrema[0].value, rfc_ctx->internal.extrema[1].value, NULL /* sign_ptr */ );

            if( is_falling_slope >= 0 && delta > rfc_ctx->hysteresis )
            {
                /* Criteria met, new turning point found.
                 * Emit maximum on falling slope as first interim turning point, 
                 * minimum as second then (and vice versa) 
                 * 1st point: internal.extrema[ is_falling_slope]
                 * 2nd point: internal.extrema[!is_falling_slope]  ==> which is *pt also
                 */
                assert( rfc_ctx->residue_cnt < rfc_ctx->residue_cap );
                rfc_ctx->residue[rfc_ctx->residue_cnt] = rfc_ctx->internal.extrema[is_falling_slope];

                rfc_ctx->internal.slope = is_falling_slope ? -1 : 1;

                /* pt is the new interim turning point */
                rfc_ctx->state = RFC_STATE_BUSY_INTERIM;
                do_append = 1;
            }
        }
    }
    else  /* if( rfc_ctx->state < RFC_STATE_BUSY_INTERIM ) */
    {
        assert( rfc_ctx->state == RFC_STATE_BUSY_INTERIM );

        /* Consecutive search for turning points */


        /* Hysteresis Filtering, check against interim turning point */
        delta = value_delta( rfc_ctx->residue[rfc_ctx->residue_cnt].value, pt->value, &slope /* sign_ptr */ );

        /* There are three scenarios possible here:
         *   1. Previous slope is continued
         *      "delta" is ignored whilst hysteresis is exceeded already.
         *      Interim turning point has just to be adjusted.
         *   2. Slope reversal, slope is greater than hysteresis
         *      Interim turning point becomes real turning point.
         *      Current point becomes new interim turning point
         *   3. Slope reversal, slope is less than or equal hysteresis
         *      Nothing to do.
         */

        /* Peak-Valley Filtering */
        /* Justify interim turning point, or add a new one (2) */
        if( slope == rfc_ctx->internal.slope )
        {
            /* Scenario (1), Continuous slope */

            /* Replace interim turning point with new extrema */
            if( rfc_ctx->residue[rfc_ctx->residue_cnt].value != pt->value )
            {
                rfc_ctx->residue[rfc_ctx->residue_cnt] = *pt;
            }
        }
        else
        {
            if( delta > rfc_ctx->hysteresis )
            {
                /* Scenario (2), Criteria met: slope != rfc_ctx->internal.slope && delta > rfc_ctx->hysteresis */

                /* Storage */
                rfc_ctx->internal.slope = slope;

                /* Handle new turning point */
                do_append = 1;
            }
            else
            {
                /* Scenario (3), Turning point found, but still in hysteresis band, nothing to do */
            }
        }
    }

    /* Handle new turning point, that is the current last point in residue */
    if( do_append )
    {
        assert( rfc_ctx->state == RFC_STATE_BUSY_INTERIM );

        /* Increment and set new interim turning point */
        assert( rfc_ctx->residue_cnt < rfc_ctx->residue_cap );
        rfc_ctx->residue[++rfc_ctx->residue_cnt] = *pt;

        /* Return new turning point */
        new_tp = &rfc_ctx->residue[rfc_ctx->residue_cnt - 1];
    }

    return new_tp;
}


/**
 * @brief      Rainflow counting core (4-point-method).
 *
 * @param      rfc_ctx  The rainflow context
 * @param[in]  flags    The flags
 */
static
void RFC_cycle_find_4ptm( rfc_ctx_s *rfc_ctx, int flags )
{
    assert( rfc_ctx );
    assert( rfc_ctx->state >= RFC_STATE_INIT && rfc_ctx->state < RFC_STATE_FINISHED );

    while( rfc_ctx->residue_cnt >= 4 )
    {
        size_t idx = rfc_ctx->residue_cnt - 4;

        RFC_value_type A = rfc_ctx->residue[idx+0].value;
        RFC_value_type B = rfc_ctx->residue[idx+1].value;
        RFC_value_type C = rfc_ctx->residue[idx+2].value;
        RFC_value_type D = rfc_ctx->residue[idx+3].value;

        if( B > C )
        {
            RFC_value_type temp = B;
            B = C;
            C = temp;
        }

        if( A > D )
        {
            RFC_value_type temp = A;
            A = D;
            D = temp;
        }

        if( A <= B && C <= D )
        {
            rfc_value_tuple_s *from = &rfc_ctx->residue[idx+1];
            rfc_value_tuple_s *to   = &rfc_ctx->residue[idx+2];

            /* Closed cycle found, process countings */
            RFC_cycle_process_counts( rfc_ctx, from, to, to + 1, flags );

            /* Remove two inner turning points (idx+1 and idx+2) */
            /* Move last turning point */
            rfc_ctx->residue[idx+1] = rfc_ctx->residue[idx+3];
            /* Move interim turning point */
            if( rfc_ctx->state == RFC_STATE_BUSY_INTERIM )
            {
                rfc_ctx->residue[idx+2] = rfc_ctx->residue[idx+4];
            }
            rfc_ctx->residue_cnt -= 2;
        }
        else break;
    }
}


/**
 * @brief         Processes counts on a closing cycle
 *
 * @param         rfc_ctx  The rainflow context
 * @param[in,out] from     The starting data point
 * @param[in,out] to       The ending data point
 * @param[in,out] next     The point next after "to"
 * @param[in]     flags    Control flags
 */
static
void RFC_cycle_process_counts( rfc_ctx_s *rfc_ctx, rfc_value_tuple_s *from, rfc_value_tuple_s *to, rfc_value_tuple_s *next, int flags )
{
    unsigned class_from, class_to;

    assert( rfc_ctx );
    assert( rfc_ctx->state >= RFC_STATE_INIT && rfc_ctx->state < RFC_STATE_FINISHED );
    assert( !rfc_ctx->class_count || ( from->value > rfc_ctx->class_offset && to->value > rfc_ctx->class_offset ) );


    /* Quantize "from" */
    class_from = QUANTIZE( rfc_ctx, from->value );

    if( class_from >= rfc_ctx->class_count ) class_from = rfc_ctx->class_count - 1;

    /* Quantize "to" */
    class_to = QUANTIZE( rfc_ctx, to->value );

    if( class_to >= rfc_ctx->class_count ) class_to = rfc_ctx->class_count - 1;
    
    /* class_from and class_to are base 0 now */

    /* Do several counts, according to "flags" */
    if( class_from != class_to )
    {
        /* Cumulate pseudo damage */
        if( flags & RFC_FLAGS_COUNT_DAMAGE )
        {
            double amplitude;
            double damage = RFC_damage_calc( rfc_ctx, class_from, class_to, &amplitude );

            /* Adding damage due to current cycle weight */
            rfc_ctx->pseudo_damage += damage / rfc_ctx->full_inc * rfc_ctx->curr_inc;
        }

        /* Rainflow matrix */
        if( rfc_ctx->rfm && ( flags & RFC_FLAGS_COUNT_RFM ) )
        {
            /* Matrix (row-major storage):
             *          t o
             *    +-------------
             *    | 0 1 2 3 4 5
             *  f | 6 7 8 9 # #
             *  r | # # # # # #
             *  o | # # # # # #
             *  m | # # # # # #
             *    | # # # # # #<-(6x6-1)
             */
            size_t idx = rfc_ctx->class_count * class_from + class_to;
            
            assert( rfc_ctx->rfm[idx] <= RFC_COUNTS_LIMIT );
            rfc_ctx->rfm[idx] += rfc_ctx->curr_inc;
        }
    }
}


/**
 * @brief      Raises an error
 *
 * @param      rfc_ctx  The rainflow context
 * @param[in]  error    The error identifier
 *
 * @return     Always false
 */
static
bool RFC_error_raise( rfc_ctx_s *rfc_ctx, int error )
{
    assert( rfc_ctx );

    rfc_ctx->state = RFC_STATE_ERROR;
    rfc_ctx->error = error;

    return false;
}


/**
 * @brief      Returns the unsigned difference of two values, sign optionally
 *             returned as -1 or 1.
 *
 * @param[in]  from      Left hand value
 * @param[in]  to        Right hand value
 * @param[out] sign_ptr  Pointer to catch sign (may be NULL)
 *
 * @return     Returns the absolute difference of given values
 */
static
RFC_value_type value_delta( RFC_value_type from_val, RFC_value_type to_val, int *sign_ptr )
{
    double delta = (double)to_val - (double)from_val;

    if( sign_ptr )
    {
        *sign_ptr = ( delta < 0.0 ) ? -1 : 1;
    }

    return (RFC_value_type)fabs( delta );
}


/**
 * @brief      (Re-)Allocate or free memory
 *
 * @param      ptr   Previous data pointer, or NULL, if unset
 * @param[in]  num   The number of elements
 * @param[in]  size  The size of one element in bytes
 * @param[in]  aim   The aim
 *
 * @return     New memory pointer or NULL if either num or size is 0
 */
static
void * RFC_mem_alloc( void *ptr, size_t num, size_t size, int aim )
{
    if( !num || !size )
    {
        if( ptr )
        {
            free( ptr );
        }
        return NULL;
    }
    else
    {
        return ptr ? realloc( ptr, num * size ) : calloc( num, size );
    }
}



/*********************************************************************************************************/
/*********************************************************************************************************/
/*********************************************************************************************************/
/*********************************************************************************************************/
/*********************************************************************************************************/
/*********************************************************************************************************/
/*********************************************************************************************************/


#if MATLAB_MEX_FILE
/**
 * MATLAB wrapper for the rainflow algorithm
 */
static
void mexRainflow( int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[] )
{
    if( nrhs != 6 )
    {
        if( !nrhs )
        {
            mexPrintf( "%s", RFC_MEX_USAGE );
            return;
        }
        mexErrMsgTxt( "Function needs exact 5 arguments!" );
    }
    else
    {
        rfc_ctx_s rfc_ctx = { sizeof(rfc_ctx_s) };
    
        const mxArray  *mxData           = prhs[0];
        const mxArray  *mxClassCount     = prhs[1];
        const mxArray  *mxClassWidth     = prhs[2];
        const mxArray  *mxClassOffset    = prhs[3];
        const mxArray  *mxHysteresis     = prhs[4];
        const mxArray  *mxResidualMethod = prhs[5];

        RFC_value_type *buffer           = NULL;
        double         *data             = mxGetPr( mxData );
        size_t          data_len         = mxGetNumberOfElements( mxData );
        unsigned        class_count      = (unsigned)( mxGetScalar( mxClassCount ) + 0.5 );
        double          class_width      = mxGetScalar( mxClassWidth );
        double          class_offset     = mxGetScalar( mxClassOffset );
        double          hysteresis       = mxGetScalar( mxHysteresis );
        int             residual_method  = (int)( mxGetScalar( mxResidualMethod ) + 0.5 );
        size_t          i;
        bool            ok;

        mxAssert( residual_method >= RFC_RES_NONE && residual_method <= RFC_RES_COUNT, 
                  "Invalid residual method!" );

        ok = RFC_init( &rfc_ctx, 
                       class_count, (RFC_value_type)class_width, (RFC_value_type)class_offset, 
                       (RFC_value_type)hysteresis, RFC_FLAGS_DEFAULT );

        if( !ok )
        {
            mexErrMsgTxt( "Error during initialization!" );
        }

        /* Cast values from double type to RFC_value_type */ 
        if( sizeof( RFC_value_type ) != sizeof(double) && data_len )  /* maybe unsafe! */
        {
            buffer = (RFC_value_type *)RFC_mem_alloc( NULL, data_len, 
                                                      sizeof(RFC_value_type), RFC_MEM_AIM_TEMP );

            if( !buffer )
            {
                RFC_deinit( &rfc_ctx );
                mexErrMsgTxt( "Error during initialization!" );
            }

            for( i = 0; i < data_len; i++ )
            {
                buffer[i] = (RFC_value_type)data[i];
            }
        }
        else buffer = (RFC_value_type*)data;

        /* Rainflow counting */

        RFC_feed( &rfc_ctx, buffer, data_len );
        RFC_finalize( &rfc_ctx, residual_method );

        /* Free temporary buffer (cast) */
        if( (void*)buffer != (void*)data )
        {
            RFC_mem_alloc( buffer, 0, 0, RFC_MEM_AIM_TEMP );
            buffer = NULL;
        }

        /* Return results */
        if( plhs )
        {
            /* Pseudo damage */
            plhs[0] = mxCreateDoubleScalar( rfc_ctx.pseudo_damage );

            /* Residue */
            if( nlhs > 1 && rfc_ctx.residue )
            {
                mxArray* re = mxCreateDoubleMatrix( rfc_ctx.residue_cnt, 1, mxREAL );
                if( re )
                {
                    size_t i;
                    double *val = mxGetPr(re);

                    for( i = 0; i < rfc_ctx.residue_cnt; i++ )
                    {
                        *val++ = (double)rfc_ctx.residue[i].value;
                    }
                    plhs[1] = re;
                }
            }

            /* Rainflow matrix (column major order) */
            if( nlhs > 2 && rfc_ctx.rfm )
            {
                mxArray* rfm = mxCreateDoubleMatrix( class_count, class_count, mxREAL );
                if( rfm )
                {
                    double *ptr = mxGetPr(rfm);
                    size_t from, to;
                    for( to = 0; to < class_count; to++ )
                    {
                        for( from = 0; from < class_count; from++ )
                        {
                            *ptr++ = (double)rfc_ctx.rfm[ from * class_count + to ] / rfc_ctx.full_inc;
                        }
                    }
                    plhs[2] = rfm;
                }
            }
        }

        /* Deinitialize rainflow context */
        RFC_deinit( &rfc_ctx );
    }
}


/**
 * The MATLAB MEX main function
 */
void mexFunction( int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[] )
{
    mexRainflow( nlhs, plhs, nrhs, prhs );
}

#endif /*MATLAB_MEX_FILE*/
