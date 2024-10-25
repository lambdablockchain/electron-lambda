/**********************************************************************
 * Copyright (c) 2014 Pieter Wuille                                   *
 * Distributed under the MIT software license, see the accompanying   *
 * file COPYING or http://www.opensource.org/licenses/mit-license.php.*
 **********************************************************************/

#include "include/secp256k1.h"
#include "util.h"
#include "bench.h"

#ifdef ENABLE_MODULE_SCHNORR
#include "include/secp256k1_schnorr.h"
#endif


typedef struct {
    secp256k1_context* ctx;
    unsigned char msg[32];
    unsigned char key[32];
} bench_sign;

static void bench_sign_setup(void* arg) {
    int i;
    bench_sign *data = (bench_sign*)arg;

    for (i = 0; i < 32; i++) {
        data->msg[i] = i + 1;
    }
    for (i = 0; i < 32; i++) {
        data->key[i] = i + 65;
    }
}

static void bench_sign_run(void* arg) {
    int i;
    bench_sign *data = (bench_sign*)arg;

    unsigned char sig[74];
    for (i = 0; i < 20000; i++) {
        size_t siglen = 74;
        int j;
        secp256k1_ecdsa_signature signature;
        CHECK(secp256k1_ecdsa_sign(data->ctx, &signature, data->msg, data->key, NULL, NULL));
        CHECK(secp256k1_ecdsa_signature_serialize_der(data->ctx, sig, &siglen, &signature));
        for (j = 0; j < 32; j++) {
            data->msg[j] = sig[j];
            data->key[j] = sig[j + 32];
        }
    }
}

#ifdef ENABLE_MODULE_SCHNORR
static void bench_schnorr_sign_run(void* arg) {
    int i,j;
    bench_sign *data = (bench_sign*)arg;

    unsigned char sig[64];
    for (i = 0; i < 20000; i++) {
        CHECK(secp256k1_schnorr_sign(data->ctx, sig, data->msg, data->key, NULL, NULL));
        for (j = 0; j < 32; j++) {
            data->msg[j] = sig[j];
            data->key[j] = sig[j + 32];
        }
    }
}
#endif

int main(void) {
    bench_sign data;

    data.ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);

    run_benchmark("ecdsa_sign", bench_sign_run, bench_sign_setup, NULL, &data, 10, 20000);
#ifdef ENABLE_MODULE_SCHNORR
    run_benchmark("schnorr_sign", bench_schnorr_sign_run, bench_sign_setup, NULL, &data, 10, 20000);
#endif

    secp256k1_context_destroy(data.ctx);
    return 0;
}
