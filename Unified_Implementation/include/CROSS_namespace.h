#ifndef CROSS_NAMESPACE_H
#define CROSS_NAMESPACE_H

#ifdef CROSS_NAMESPACE

#define CROSS_CONCAT_INNER(prefix, func) prefix ## func
#define CROSS_CONCAT(prefix, func) CROSS_CONCAT_INNER(prefix, func)

/* CROSS API */
#define CROSS_keygen    CROSS_CONCAT(CROSS_NAMESPACE, CROSS_keygen)
#define CROSS_sign      CROSS_CONCAT(CROSS_NAMESPACE, CROSS_sign)
#define CROSS_verify    CROSS_CONCAT(CROSS_NAMESPACE, CROSS_verify)

/* Merkle Tree */
#define tree_root       CROSS_CONCAT(CROSS_NAMESPACE, tree_root)
#define tree_proof      CROSS_CONCAT(CROSS_NAMESPACE, tree_proof)
#define recompute_root  CROSS_CONCAT(CROSS_NAMESPACE, recompute_root)

/* Seed Tree */
#define seed_leaves     CROSS_CONCAT(CROSS_NAMESPACE, seed_leaves)
#define seed_path       CROSS_CONCAT(CROSS_NAMESPACE, seed_path)
#define rebuild_leaves  CROSS_CONCAT(CROSS_NAMESPACE, rebuild_leaves)
#define gen_seed_tree   CROSS_CONCAT(CROSS_NAMESPACE, gen_seed_tree)
#define rebuild_tree    CROSS_CONCAT(CROSS_NAMESPACE, rebuild_tree)
#define pseed           CROSS_CONCAT(CROSS_NAMESPACE, pseed)
#define psalt           CROSS_CONCAT(CROSS_NAMESPACE, psalt)
#define ptree           CROSS_CONCAT(CROSS_NAMESPACE, ptree)

/* Pack/Unpack */
#define pack_fp_vec             CROSS_CONCAT(CROSS_NAMESPACE, pack_fp_vec)
#define pack_fp_syn             CROSS_CONCAT(CROSS_NAMESPACE, pack_fp_syn)
#define pack_fz_vec             CROSS_CONCAT(CROSS_NAMESPACE, pack_fz_vec)
#define pack_fz_rsdp_g_vec      CROSS_CONCAT(CROSS_NAMESPACE, pack_fz_rsdp_g_vec)
#define unpack_fp_vec           CROSS_CONCAT(CROSS_NAMESPACE, unpack_fp_vec)
#define unpack_fp_syn           CROSS_CONCAT(CROSS_NAMESPACE, unpack_fp_syn)
#define unpack_fz_vec           CROSS_CONCAT(CROSS_NAMESPACE, unpack_fz_vec)
#define unpack_fz_rsdp_g_vec    CROSS_CONCAT(CROSS_NAMESPACE, unpack_fz_rsdp_g_vec)

/* Internal functions in CROSS.c */
#define expand_pk       CROSS_CONCAT(CROSS_NAMESPACE, expand_pk)
#define expand_sk       CROSS_CONCAT(CROSS_NAMESPACE, expand_sk)

/* CSPRNG and Hash */
#define hash                            CROSS_CONCAT(CROSS_NAMESPACE, hash)
#define hash_par                        CROSS_CONCAT(CROSS_NAMESPACE, hash_par)
#define expand_digest_to_fixed_weight   CROSS_CONCAT(CROSS_NAMESPACE, expand_digest_to_fixed_weight)
#define csprng_fp_vec                   CROSS_CONCAT(CROSS_NAMESPACE, csprng_fp_vec)
#define csprng_fp_vec_chall_1           CROSS_CONCAT(CROSS_NAMESPACE, csprng_fp_vec_chall_1)
#define csprng_fp_mat                   CROSS_CONCAT(CROSS_NAMESPACE, csprng_fp_mat)
#define csprng_fz_vec                   CROSS_CONCAT(CROSS_NAMESPACE, csprng_fz_vec)
#define csprng_fz_inf_w                 CROSS_CONCAT(CROSS_NAMESPACE, csprng_fz_inf_w)
#define csprng_fz_mat                   CROSS_CONCAT(CROSS_NAMESPACE, csprng_fz_mat)

#endif /* CROSS_NAMESPACE */

#endif /* CROSS_NAMESPACE_H */
