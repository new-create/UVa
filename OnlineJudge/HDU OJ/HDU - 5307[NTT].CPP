#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <algorithm>
using namespace std;

typedef unsigned int UINT32;
typedef long long INT64;
class TOOL_NTT {
public:
#define MAXN 262144
    const INT64 P = 50000000001507329LL; // prime m = kn+1
    const INT64 G = 3;
    INT64 wn[20];
    INT64 s[MAXN], d1[MAXN], d2[MAXN], y[MAXN];
    TOOL_NTT() {
        for (int i = 0; i < 20; i++)
            wn[i] = mod_pow(G, (P-1) / (1<<i), P);
    }
    INT64 mod_mul(INT64 a, INT64 b, INT64 mod) {
        return (a*b - (long long)(a/(long double)mod*b+1e-3)*mod+mod)%mod;
        //      INT64 ret = 0;
        //      for (a = a >= mod ? a%mod : a, b = b >= mod ? b%mod : b; b != 0; b>>=1, a <<= 1, a = a >= mod ? a - mod : a) {
        //          if (b&1) {
        //              ret += a;
        //              if (ret >= mod)
        //                  ret -= mod;
        //          }
        //      }
        //      return ret;
    }
    INT64 mod_pow(INT64 n, INT64 e, INT64 m) {
        INT64 x = 1;
        for (n = n >= m ? n%m : n; e; e >>= 1) {
            if (e&1)
                x = mod_mul(x, n, m);
            n = mod_mul(n, n, m);
        }
        return x;
    }
    int NumberOfBitsNeeded(int PowerOfTwo) {
        for (int i = 0;; ++i) {
            if (PowerOfTwo & (1 << i)) {
                return i;
            }
        }
    }
    inline UINT32 FastReverseBits(UINT32 a, int NumBits) {
        a = ( ( a & 0x55555555U ) << 1 ) | ( ( a & 0xAAAAAAAAU ) >> 1 ) ;
        a = ( ( a & 0x33333333U ) << 2 ) | ( ( a & 0xCCCCCCCCU ) >> 2 ) ;
        a = ( ( a & 0x0F0F0F0FU ) << 4 ) | ( ( a & 0xF0F0F0F0U ) >> 4 ) ;
        a = ( ( a & 0x00FF00FFU ) << 8 ) | ( ( a & 0xFF00FF00U ) >> 8 ) ;
        a = ( ( a & 0x0000FFFFU ) << 16 ) | ( ( a & 0xFFFF0000U ) >> 16 ) ;
        return a >> (32 - NumBits);
    }
    void NTT(int on, INT64 *In, INT64 *Out, int n) {
        int NumBits = NumberOfBitsNeeded(n);
        for (int i = 0; i < n; ++i)
            Out[FastReverseBits(i, NumBits)] = In[i];
        for(int h = 2, id = 1; h <= n; h <<= 1, id++) {
            for(int j = 0; j < n; j += h) {
                INT64 w = 1, u, t;
                int block = h/2, blockEnd = j + h/2;
                for(int k = j; k < blockEnd; k++) {
                    u = Out[k], t = mod_mul(w, Out[k+block], P);
                    Out[k] = u + t;
                    Out[k + block] = u - t + P;
                    if (Out[k] >= P)        Out[k] -= P;
                    if (Out[k+block] >= P)  Out[k+block] -= P;
                    w = mod_mul(w, wn[id], P);
                }
            }
        }
        if (on == 1) {
            for (int i = 1; i < n/2; i++)
                swap(Out[i], Out[n-i]);
            INT64 invn = mod_pow(n, P-2, P);
            for (int i = 0; i < n; i++)
                Out[i] = mod_mul(Out[i], invn, P);
        }
    }
    void convolution(INT64 *a, INT64 *b, int n, INT64 *c) {
        NTT(0, a, d1, n);
        s[0] = b[0];
        for (int i = 1; i < n; ++i)
            s[i] = b[n-i];
        NTT(0, s, d2, n);
        for (int i = 0; i < n; i++)
            s[i] = mod_mul(d1[i], d2[i], P);
        NTT(1, s, c, n);
    }
} tool;

INT64 a[MAXN], b[MAXN], c[MAXN];
int A[MAXN], sum[MAXN];
long long ret[MAXN], pr[262144];
int main() {
    pr[0] = 0;
    for (int i = 1; i < MAXN; i++)
        pr[i] = pr[i-1] + (long long)i*(i+1)/2;
    int testcase, n, m, s;
    scanf("%d", &testcase);
    while (testcase--) {
        scanf("%d", &n);
        for (int i = 1; i <= n; i++)
            scanf("%d", &A[i]);
        sum[0] = 0;
        for (int i = 1; i <= n; i++)
            sum[i] = sum[i-1] + A[i];
        
        s = sum[n];
        memset(ret, 0, sizeof(ret[0]) * (s+1));
        for (m = 1; m <= (s<<1); m <<= 1);
        
        memset(a, 0, sizeof(a[0])*m);
        memset(b, 0, sizeof(b[0])*m);
        for (int i = 1; i <= n; i++) {
            a[sum[i]] += i;
            b[sum[i-1]] ++;
        }
        tool.convolution(a, b, m, c);
        for (int i = 1; i < m; i++)
            ret[i] += c[i];

        memset(a, 0, sizeof(a[0])*m);
        memset(b, 0, sizeof(b[0])*m);
        for (int i = 1; i <= n; i++) {
            a[sum[i]] ++;
            b[sum[i-1]] += i-1;
        }
        tool.convolution(a, b, m, c);
        for (int i = 1; i <= s; i++)
            ret[i] -= c[i];
        for (int i = 1, z = 0; i <= n+1; i++) {
            if (i == n+1 || A[i] != 0)
                ret[0] += pr[z], z = 0;
            else
                z++;
        }        
        for (int i = 0; i <= s; i++) {
            printf("%lld\n", ret[i]);
        }
    }
    return 0;
}
