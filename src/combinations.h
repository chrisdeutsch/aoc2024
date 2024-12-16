#ifndef COMBINATIONS_H_
#define COMBINATIONS_H_

template <typename T> class CombinationIterator {
  public:
    CombinationIterator(const T begin0, const T end0, const T begin1, const T end1)
        : begin0(begin0), end0(end0), begin1(begin1), end1(end1), first(begin0), second(begin1) {}

    CombinationIterator &next() { return *this; }

  private:
    const T begin0, end0, begin1, end1;
    T first;
    T second;
};

#endif
