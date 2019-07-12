#include <iostream>

////
// List and Nil
template <typename X, typename Xs>
struct List {
    using First = X;
    using Rest = Xs;
    static void print() {
        std::cout<<"(";
        First::print();
        std::cout<<",";
        Rest::print();
        std::cout<<")";
    }
};

struct Nil {
    using First = Nil;
    static void print() {
        std::cout<<"Nil";
    }
};


////
// List concatenation
template <typename L1, typename L2>
struct ListConcat {
    using L1First = typename L1::First;
    using L1Rest = typename L1::Rest;
    using L1RestL2 = typename ListConcat<L1Rest, L2>::Result;
    using Result = List<L1First, L1RestL2>;
};

template <typename L2>
struct ListConcat<Nil, L2> {
    using Result = L2;
};

template <typename L>
struct ListConcatAll {
    using FirstList = typename L::First;
    using RestLists = typename L::Rest;
    using RestConcat = typename ListConcatAll<RestLists>::Result;
    using Result = typename ListConcat<FirstList, RestConcat>::Result;
};

template <>
struct ListConcatAll<Nil> {
    using Result = Nil;
};


////
// Booleans, Not, Or, And
struct True { static void print() { std::cout<<"True"; } };
struct False { static void print() { std::cout<<"False"; } };;

template<typename B> struct Not;
template<> struct Not<True> { using Result = False; };
template<> struct Not<False> { using Result = True; };

template<typename B1, typename B2> struct Or;
template <> struct Or<True, True> { using Result = True; };
template <> struct Or<True, False> { using Result = True; };
template <> struct Or<False, True> { using Result = True; };
template <> struct Or<False, False> { using Result = False; };

template<typename B1, typename B2> struct And;
template <> struct And<True, True> { using Result = True; };
template <> struct And<True, False> { using Result = False; };
template <> struct And<False, True> { using Result = False; };
template <> struct And<False, False> { using Result = False; };

template <typename L> struct AnyTrue;
template <typename Rest> struct AnyTrue<List<True, Rest> > { using Result = True; };
template <typename Rest> struct AnyTrue<List<False, Rest> > { using Result = typename AnyTrue<Rest>::Result; };
template <> struct AnyTrue<Nil> { using Result = False; };


////
// Natural numbers
// with printing!
struct Z { static void print() { std::cout<<0; } static void print(int acc) { std::cout<<acc; } };;;
template<typename N> struct S : N { static void print() { print(0); } static void print(int acc) { N::print(acc+1); } };


////
// Number comparisons

// equal
template<typename S1, typename S2> struct PeanoEqual;
template<> struct PeanoEqual<Z, Z> { using Result = True; };
template<typename S1> struct PeanoEqual<Z, S1> { using Result = False; };
template<typename S1> struct PeanoEqual<S1, Z> { using Result = False; };
template<typename SS1, typename SS2> struct PeanoEqual<S<SS1>, S<SS2> > { using Result = typename PeanoEqual<SS1, SS2>::Result; };

// less than
template<typename S1, typename S2> struct PeanoLT;
template<> struct PeanoLT<Z, Z> { using Result = False; };
template<typename S1> struct PeanoLT<Z, S1> { using Result = True; };
template<typename S1> struct PeanoLT<S1, Z> { using Result = False; };
template<typename SS1, typename SS2> struct PeanoLT<S<SS1>, S<SS2> > { using Result = typename PeanoLT<SS1, SS2>::Result; };

// absolute difference
template <typename S1, typename S2> struct PeanoAbsDiff;
template <> struct PeanoAbsDiff<Z, Z> { using Result = Z; };
template <typename S1> struct PeanoAbsDiff<Z, S<S1> > { using Result = S<S1>; };
template <typename S1> struct PeanoAbsDiff<S<S1>, Z > { using Result = S<S1>; };
template <typename SS1, typename SS2> struct PeanoAbsDiff<S<SS1>, S<SS2> > { using Result = typename PeanoAbsDiff<SS1, SS2>::Result; };

// integers from n to 0
template <typename S1> struct Range;
template <> struct Range<Z> { using Result=List<Z, Nil>; };
template <typename SS1> struct Range<S<SS1> > { using Result = List<S<SS1>, typename Range<SS1>::Result>; };


////
// Functions
template <typename F, typename A> struct Apply;

// push X in front of L
template <typename L> struct Conj1;
template <typename L, typename X> struct Apply<Conj1<L>, X> { using Result=List<X, L>; };

// Map
template <typename F, typename Xs> struct Map;
template <typename F> struct Map<F, Nil> { using Result=Nil; };
template <typename F, typename Xs> struct Map {
    using XRest = typename Xs::Rest;
    using Y = typename Apply<F, typename Xs::First>::Result;
    using YRest = typename Map<F, XRest>::Result;
    using Result=List<Y, YRest>;
};

// Map f over list and concatenate results together
template <typename F, typename Xs> struct MapCat;
template <typename F> struct MapCat<F, Nil> { using Result=Nil; };
template <typename F, typename Xs> struct MapCat {
    using Chunks = typename Map<F, Xs>::Result;
    using Result = typename ListConcatAll<Chunks>::Result;
};

// Filter a list
template <typename B, typename X, typename Xs> struct AppendIf;
template <typename X, typename Xs> struct AppendIf<True, X, Xs> { using Result=List<X, Xs>; };
template <typename X, typename Xs> struct AppendIf<False, X, Xs> { using Result=Xs; };

template <typename F, typename Xs> struct Filter;
template <typename F> struct Filter<F, Nil> { using Result=Nil; };
template <typename F, typename Xs> struct Filter {
    using X=typename Xs::First;
    using XRest=typename Xs::Rest;
    using XRestFiltered=typename Filter<F, XRest>::Result;
    using Pred=typename Apply<F, X>::Result;
    using Result=typename AppendIf<Pred, X, XRestFiltered>::Result;
};


// Her majesty
template <typename X, typename Y> struct Queen {
    static void print() {
        std::cout<<"Queen ";
        X::print();
        std::cout<<" ";
        Y::print();
    }
};
template <typename X> struct Queen1;
template <typename X, typename Y> struct Apply<Queen1<X>, Y> { using Result=Queen<X, Y>; };

// A list of queens in a row x with y from 0 to n
template <typename N, typename X> struct QueensInRow {
    using Ys=typename Range<N>::Result;
    using Result=typename Map<Queen1<X>, Ys>::Result;
};


// Does queen a threaten queen b
template <typename A, typename B> struct Threatens;
template <typename AX, typename AY, typename BX, typename BY>
struct Threatens<Queen<AX, AY>, Queen<BX, BY> > {
    using Xeq=typename PeanoEqual<AX, BX>::Result;
    using Yeq=typename PeanoEqual<AY, BY>::Result;
    using dX=typename PeanoAbsDiff<AX, BX>::Result;
    using dY=typename PeanoAbsDiff<AY, BY>::Result;
    using XYeq=typename Or<Xeq, Yeq>::Result;
    using Deq=typename PeanoEqual<dX, dY>::Result;
    using Result=typename Or<XYeq, Deq>::Result;
};

template <typename A> struct Threatens1;
template <typename A, typename B> struct Apply<Threatens1<A>, B> { using Result=typename Threatens<A, B>::Result; };


// Is queen B compatible with all queens As
template <typename Config, typename Queen>
struct Safe {
    using M1=typename Map<Threatens1<Queen>, Config>::Result;
    using T1=typename AnyTrue<M1>::Result;
    using Result=typename Not<T1>::Result;
};

template <typename Config> struct Safe1;
template <typename Config, typename Queen> struct Apply<Safe1<Config>, Queen> { using Result=typename Safe<Config, Queen>::Result; };


// Add a queen with the given x coordinate to a legal configuration, returning a set of legal configurations
template <typename N, typename X, typename Config>
struct AddQueen {
    using Candidates=typename QueensInRow<N, X>::Result;
    using Filtered=typename Filter<Safe1<Config>, Candidates>::Result;
    using Result=typename Map<Conj1<Config>, Filtered>::Result;
};

template <typename N, typename X> struct AddQueen2;
template <typename N, typename X, typename Config> struct Apply<AddQueen2<N, X>, Config> {
    using Result=typename AddQueen<N, X, Config>::Result;
};


// Add a queen at x to every configuration returning a set of legal configurations
template <typename N, typename X, typename Configs>
struct AddQueenToAll {
    using Result=typename MapCat<AddQueen2<N, X>, Configs>::Result;
};


// Add queens recursively
template <typename Pred, typename N, typename X, typename Configs> struct AddQueensIf;
template <typename N, typename X, typename Configs> struct AddQueensIf<False, N, X, Configs> {
    using Result=Configs;
};

template <typename N, typename X, typename Configs> struct AddQueens;
template <typename N, typename X, typename Configs> struct AddQueensIf<True, N, X, Configs> {
    using Configs2=typename AddQueenToAll<N, X, Configs>::Result;
    using Result=typename AddQueens<N, S<X>, Configs2>::Result;
};

template <typename N, typename X, typename Configs> struct AddQueens {
    using Pred=typename PeanoLT<X, S<N> >::Result;
    using Result=typename AddQueensIf<Pred, N, X, Configs>::Result;
};


// Solve
template <typename N>
struct Solution {
    using Configs=typename AddQueens<N, Z, List<Nil, Nil> >::Result;
    using Result=typename Configs::First;
};


// Print list of lists one per line
template <typename L>
struct PrintLists {
    using LFirst=typename L::First;
    using LRest=typename L::Rest;
    using PrintRest=PrintLists<LRest>;
    static void print() {
        LFirst::print();
        std::cout<<"\n";
        PrintRest::print();
    }
};

template <>
struct PrintLists<Nil> {
    static void print() {}
};

int main(int argc, char **argv)
{
    using N0 = Z;
    using N1 = S<Z>;
    using N2 = S<N1>;
    using N3 = S<N2>;
    using N4 = S<N3>;
    using N5 = S<N4>;
    using N6 = S<N5>;
    using N7 = S<N6>;
    using N8 = S<N7>;
    using FinalSolution=Solution<N7>;
    // Some debug stuff we don't need now, but left here for you to play with
    /*
    using N35Diff = typename PeanoAbsDiff<N3, N5>::Result;

    using Legal = typename PeanoEqual<S<Z>, S<Z> >::Result;
    //using Illegal = typename PeanoEqual<True, List<Z, False> >::Result;

    using A = Nil; // null list
    using B = List<A, Nil>; // list of one element - Nil.
    using C = List<A, B>; // list of two elements
    using D = List<C, List<C, List<C, Nil> > >; // list of three lists of two elements
    using E = typename ListConcatAll<D>::Result; // list of six elements

    using R=typename Range<N3>::Result;

    C::print();
    std::cout<<"\n";
    D::print();
    std::cout<<"\n";
    N8::print();
    std::cout<<"\n";
    N35Diff::print();
    std::cout<<"\n";
    Legal::print();
    //Illegal::print();
    std::cout<<"\n";
    R::print();
    std::cout<<"\n";
    */
    PrintLists<FinalSolution::Configs>::print();
    std::cout<<"\n";
    return 0;
}
