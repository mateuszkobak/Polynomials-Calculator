/** @file
 *
 * Implementacja klasy wielomianów
 */


#include "poly.h"
#include "utils.h"
#include <assert.h>


void PolyDestroy(Poly *p) {
    if (p == NULL) return;
    for(unsigned i = 0; i < p->monos_count; i++) {
        MonoDestroy( &(p->monos[i]) );
    }
    if (p->monos_count > 0) free(p->monos);
    return;
}


Poly PolyClone(const Poly *p) {
    Poly result;

    if(p->monos_count == 0) {
        result.monos_count = 0;
        result.monos = NULL;
        result.constant_value = p->constant_value;
    }
    else {
        result.constant_value = 1;
        result.monos_count = p->monos_count;
        result.monos = malloc(p->monos_count * sizeof(Mono));
        assert(result.monos != NULL);
        for (unsigned i = 0; i < result.monos_count; i++) {
            result.monos[i] = MonoClone( &(p->monos[i]) );
        }
    }
    return result;
}


/**
 * Sprawdza czy jedomian jest zerowy
 * @param[in] m : jednomian
 * @return Czy jednomian jest zerowy ?
 */
bool MonoIsZero(Mono *m) {
    return PolyIsZero(&m->p);
}


/**
 * Jeśli wielomian jest stały, to sprowadza go
 * do pożądanej formy reprezentowania takich wielomianów
 * @param[in] p : wielomian
 * @return Wyekstraktowany wielomian stały, lub p
 */
Poly ExtractPolyCoeff(Poly *p) {
    if (p->monos_count == 1 && p->monos[0].exp == 0) {
        Poly q = ExtractPolyCoeff(&p->monos[0].p);
        if (PolyIsCoeff(&q)) {
            PolyDestroy(p);
            return q;
        }
    }
    return *p;
}


/**
 * Dodaje do siebie dwa wielomiany, z czego jeden jest stały
 * @param p - wielomian
 * @param coeff - wielomian stały
 * @return
 */
Poly PolyAddPolyCoeff(const Poly *p, const Poly *coeff) {
    Poly result;

    if (PolyIsZero(coeff)) result = PolyClone(p);

    else if (p->monos[0].exp > 0) {
        Mono m = MonoFromPoly(coeff, 0);
        result.constant_value = 1;
        result.monos_count = p->monos_count + 1;
        result.monos = malloc( (p->monos_count + 1) * sizeof(Mono) );
        assert(result.monos != NULL);
        for (unsigned i = 0; i < p->monos_count; i++) {
            result.monos[i + 1] = MonoClone(&p->monos[i]);
        }
        result.monos[0] = m;
    }
    else {
        Poly pom = PolyAdd(coeff, &p->monos[0].p);
        if (PolyIsZero(&pom)) {
            result.constant_value = 1;
            result.monos_count = p->monos_count - 1;
            result.monos = malloc( (p->monos_count - 1) * sizeof(Mono) );
            assert(result.monos != NULL);
            for (unsigned i = 0; i < p->monos_count - 1; i++) {
                result.monos[i] = MonoClone(&p->monos[i + 1]);
            }
        }
        else {
            Mono m = MonoFromPoly(&pom, 0);
            result.constant_value = 1;
            result.monos_count = p->monos_count;
            result.monos = malloc( p->monos_count * sizeof(Mono) );
            assert(result.monos != NULL);
            for (unsigned i = 1; i < p->monos_count; i++) {
                result.monos[i] = MonoClone(&p->monos[i]);
            }
            result.monos[0] = m;
        }
    }
    return result;
}


Poly PolyAdd(const Poly *p, const Poly *q) {
    Poly result;

    if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
        result = PolyFromCoeff(p->constant_value + q->constant_value);
    }
    else if (PolyIsCoeff(p)) {
        result = PolyAddPolyCoeff(q, p);
    }
    else if (PolyIsCoeff(q)) {
        result = PolyAddPolyCoeff(p, q);
    }
    else {
        unsigned pi = 0;
        unsigned qi = 0;
        Mono prepareMonos [p->monos_count + q->monos_count];
        unsigned currCount = 0;

        while(pi < p->monos_count || qi < q->monos_count) {
            Mono res;
            if (pi == p->monos_count) {
                res = MonoClone(&q->monos[qi]);
                qi++;
            }
            else if (qi == q->monos_count) {
                res = MonoClone(&p->monos[pi]);
                pi++;
            }
            else {
                Mono pm = p->monos[pi];
                Mono qm = q->monos[qi];
                if (pm.exp < qm.exp) {
                    res = MonoClone(&pm);
                    pi++;
                }
                else if (pm.exp > qm.exp) {
                    res = MonoClone(&qm);
                    qi++;
                }
                else {
                    res.exp = pm.exp;
                    res.p = PolyAdd(&pm.p, &qm.p);
                    pi++;
                    qi++;
                }
            }

            if (!MonoIsZero(&res)) {
                prepareMonos[currCount] = res;
                currCount++;
            }
            else {
                MonoDestroy(&res);
            }
        }
        if (currCount == 0)
            return PolyFromCoeff(0);
        else {
            result.monos = malloc(currCount * sizeof(Mono));
            assert(result.monos != NULL);
            for (unsigned i = 0; i < currCount; i++) {
                result.monos[i] = prepareMonos[i];
            }

            result.monos_count = currCount;
            result.constant_value = 1;
        }
    }
    Poly extr = ExtractPolyCoeff(&result);
    return extr;
}


/**
 * Funkcja scalająca dwie posortowane części tablicy
 * Funkcj pomocnicza funkcji MergeSort
 * @param begin
 * @param end
 * @param monos
 */
void Merge (int begin, int end, Mono *monos) {
    int middle = (begin + end) /  2;
    int i = begin;
    int j = middle + 1;
    int curr = 0;
    Mono temp [end - begin + 1];

    while (curr < end - begin + 1) {
        if (i == middle + 1) {
            temp[curr] = monos[j];
            j++;
        }
        else if (j == end + 1) {
            temp[curr] = monos[i];
            i++;
        }
        else if (monos[i].exp < monos[j].exp) {
            temp[curr] = monos[i];
            i++;
        }
        else {
            temp[curr] = monos[j];
            j++;
        }

        curr++;
    }
    for (int k = 0; k <= end - begin; k++) {
        monos[begin + k] = temp[k];
    }
}


/**
 * Sortowanie przez scalanie na danej tablicy
 * @param begin
 * @param end
 * @param monos
 */
void MonoSort(int begin, int end, Mono *monos) {

    if (end - begin == 0) {
        return;
    }
    else {
        int middle = (end + begin) / 2;
        MonoSort(begin, middle, monos);
        MonoSort(middle + 1, end, monos);
        Merge(begin, end, monos);
    }

}


/**
 * Dodaje a następnie usuwa dwa wielomiany
 * @param p
 * @param q
 * @return
 */
Poly PolyAddAndDestroy(Poly *p, Poly *q) {
    Poly result = PolyAdd(p, q);
    PolyDestroy(p);
    PolyDestroy(q);
    return result;
}


Poly PolyAddMonos(unsigned count, const Mono *monos) {
    Poly result;
    MonoSort(0, count - 1, monos);
    Mono prepareMonos [count];
    unsigned currCount = 0;
    unsigned i = 0;

    while(i < count) {
        Mono m = monos[i];
        unsigned j = i + 1;
        while(j < count && monos[i].exp == monos[j].exp) {
            Poly pom = PolyAddAndDestroy(&m.p, &monos[j].p);
            m.p = pom;
            j++;
        }
        i = j;

        if (!MonoIsZero(&m)) {
            prepareMonos[currCount] = m;
            currCount++;
        }
        else {
            MonoDestroy(&m);
        }
    }
    if(currCount == 0)
        return PolyFromCoeff(0);
    result.monos = malloc(currCount * sizeof(Mono));
    assert(result.monos != NULL);
    for (unsigned j = 0; j < currCount; j++) {
        result.monos[j] = prepareMonos[j];
    }

    result.monos_count = currCount;
    result.constant_value = 1;

    //free(monos);
    return ExtractPolyCoeff(&result);
}


/**
 * Mnoży dwa jednomiany
 * @param m
 * @param n
 * @return
 */
Mono MonoMul (Mono m, Mono n) {
    Mono result;
    result.exp = m.exp + n.exp;
    result.p = PolyMul(&m.p, &n.p);
    return result;
}


/**
 * Determinuje ile pamięci trzeba zarezerwować
 * mnożąc dany wielomian przez jakiś inny
 * @param p
 * @return
 */
unsigned PolySpace(const Poly *p) {
    if (PolyIsZero(p)) return 0;
    if (PolyIsCoeff(p)) return 1;
    return p->monos_count;
}


Poly PolyMul(const Poly *p, const Poly *q) {

    if (PolyIsZero(p) || PolyIsZero(q)) return PolyFromCoeff(0);

    Poly result;

    if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
       result = PolyFromCoeff(p->constant_value * q->constant_value);
    }
    else {
        Mono *prepareMonos = malloc((PolySpace(p) * PolySpace(q)) * sizeof(Mono));
        assert(prepareMonos != NULL);

        if (PolyIsCoeff(p)) {
            for (unsigned i = 0; i < q->monos_count; i++) {
                prepareMonos[i] = MonoMul(q->monos[i], MonoFromPoly(p, 0));
            }
        }
        if (PolyIsCoeff(q)) {
            for (unsigned i = 0; i < p->monos_count; i++) {
                prepareMonos[i] = MonoMul(p->monos[i], MonoFromPoly(q, 0));
            }
        }
        else {
            for (unsigned i = 0; i < p->monos_count; i++) {
                for (unsigned j = 0; j < q->monos_count; j++) {
                    prepareMonos[q->monos_count * i + j] = MonoMul(p->monos[i], q->monos[j]);
                }
            }

        }

        result = PolyAddMonos(PolySpace(p) * PolySpace(q), prepareMonos);
        free(prepareMonos);
    }

    return ExtractPolyCoeff(&result);
}


Poly PolyNeg(const Poly *p) {
    Poly neg = PolyFromCoeff(-1);
    return PolyMul(p, &neg);
}


Poly PolySub(const Poly *p, const Poly *q) {
    Poly negQ = PolyNeg(q);
    Poly pom =  PolyAdd(p, &negQ);
    PolyDestroy(&negQ);
    return pom;
}


int maximum(unsigned count, int *m) {
    int max = 0;
    for (unsigned i = 0; i < count; i++) {
        if (m[i] > max) max = m[i];
    }
    return max;
}


poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx) {
    if (PolyIsZero(p)) return -1;
    if (var_idx > 0) {
        int degs [p->monos_count];
        for(unsigned i = 0; i < p->monos_count; i++) {
            degs[i] = PolyDegBy(&p->monos[i].p, var_idx - 1);
        }
        return maximum(p->monos_count, degs);
    }
    else {
        int max = 0;
        for (unsigned i = 0; i < p->monos_count; i++) {
            if (p->monos[i].exp > max) max = p->monos[i].exp;
        }
        return max;
    }
}


poly_exp_t PolyDeg(const Poly *p) {
    if (PolyIsZero(p)) return -1;
    if (PolyIsCoeff(p)) {
        return 0;
    }
    else {
        int degs [p->monos_count];
        for(unsigned i = 0; i < p->monos_count; i++) {
            degs[i] = PolyDeg(&p->monos[i].p) + p->monos[i].exp;
        }
        return maximum(p->monos_count, degs);
    }
}

/**
 * Sprawdza, czy dwa jednomiany są sobie równe.
 * @param m
 * @param n
 * @return
 */
bool MonoIsEq(Mono m, Mono n) {
    if (m.exp == n.exp && PolyIsEq(&m.p, &n.p)) return true;
    else return false;
}


bool PolyIsEq(const Poly *p, const Poly *q) {
    if (p->monos_count != q->monos_count) return false;
    else {
        if (PolyIsCoeff(p)) {
            if (p->constant_value == q->constant_value) return true;
            else return false;
        }
        else {
            for (unsigned i = 0; i < p->monos_count; i++) {
                if(!MonoIsEq(p->monos[i], q->monos[i])) return false;
            }
            return true;
        }
    }
}


/**
 * Potęgowanie w czasie logarytmicznym.
 * @param x
 * @param power
 * @return
 */
poly_coeff_t Pow(poly_coeff_t x, poly_exp_t power) {
    if (power == 0) return 1;
    if (power % 2 == 0) return Pow(x * x, power/2);
    else return x * Pow(x * x, power/2);
}


Poly PolyAt(const Poly *p, poly_coeff_t x) {
    if (PolyIsCoeff(p)) return PolyClone(p);
    else {
        Poly result = PolyFromCoeff(0);
        for(unsigned i = 0; i < p->monos_count; i++) {
            Poly coeff = PolyFromCoeff(Pow(x, p->monos[i].exp));
            Poly q = PolyMul(&p->monos[i].p, &coeff);
            result = PolyAddAndDestroy(&result, &q);
        }

        return result;
    }
}

/**
 * Podnosi wielomian do potęgi power
 * @param p
 * @param power
 * @return
 */
Poly PolyPower(const Poly *p, poly_exp_t power) {
    if (PolyIsZero(p)) {
        if (power == 0) {
            return PolyFromCoeff(1);
        }
        else {
            return PolyFromCoeff(0);
        }
    }
    if (power == 0) return PolyFromCoeff(1);
    Poly result;
    Poly pom = PolyMul(p, p);
    Poly pom2 = PolyPower(&pom, power/2);
    if (power % 2 == 0) {
        result = PolyClone(&pom2);
    }
    else {
        result = PolyMul(p, &pom2);
    }
    PolyDestroy(&pom);
    PolyDestroy(&pom2);
    return result;

}

/**
 * Pomocnicza funkcja do funkcji PolyCompose implementująca rekurencyję.
 * @param p
 * @param count
 * @param x
 * @param level
 * @return
 */
Poly PolyComposeRec(const Poly *p, unsigned count, const Poly x[], unsigned level) {
    if (PolyIsCoeff(p)) return PolyClone(p);

    Poly result = PolyFromCoeff(0);

    Poly factor;
    if (count > level) {
        factor = x[level];
    }
    else {
        factor = PolyFromCoeff(0);
    }

    for (unsigned i = 0; i < p->monos_count; i++) {
        Mono m = p->monos[i];
        Poly pom1 = PolyPower(&factor, m.exp);
        Poly pom2 = PolyComposeRec(&m.p, count, x, level + 1);
        Poly pom3 = PolyMul(&pom1, &pom2);
        PolyDestroy(&pom1);
        PolyDestroy(&pom2);
        result = PolyAddAndDestroy(&result, &pom3);
    }


    return result;
}

Poly PolyCompose(const Poly *p, unsigned count, const Poly x[]) {
    return PolyComposeRec(p, count, x, 0);
}










