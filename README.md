# Kyber-mini
Toy version of Kyber following FIPS 203 recommendations

# Tests

## NTT ![Tests](https://github.com/gabauzit/Kyber-mini/workflows/Tests%20NTT%20Kyber/badge.svg)

Test 1 : ${\rm NTT}(0)=0$

Test 2 : ${\rm NTT}^{-1}(0)=0$

Test 3 : ${\rm NTT}({\rm NTT}^{-1}(f))=f$

Test 4 : ${\rm NTT}(a+b) = {\rm NTT}(a)+{\rm NTT}(b)$

Test 5 : ${\rm NTT}(a*b) = {\rm NTT}(a) \times_{{\rm NTT}} {\rm NTT}(b)$