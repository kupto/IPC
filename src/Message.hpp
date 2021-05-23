/*
 * Inter Process Communication (IPC) Message utility.
 *
 * Copyright (c) 2021, Kupto
 *
 * Basically a Signal<> with additional data, transfered alongside the flag.
 * Beware that there is no notion of repeat-count.  The only guarantee is to
 * always receive the complete value set atomically. Orders calls to raise(),
 * query(), and update() member methods.
 *
 * This is free software; published under the MIT license.
 * See the LICENSE file.
 */

#include "Signal.hpp"

#pragma once

namespace IPC {

  /**
   * class BaseMessage<> - parent of both Message<> and UpdatedMessage<>
   *
   * Inherits from some kind of a Signal. Allow to store a message (template
   * parameter pack) during the raise() of the Signal and to retrieve it via
   * the overloaded query() member.
   *
   * TODO: Consider allowing the parameter less query()
   * TODO: Allow explicitly only value-types in message Types... ?
   */
  template <typename SignalType, typename ...Types>
  class BaseMessage : public SignalType {

  protected:

#   if 1 /// Custom std::tuple<>
    /// Implementation, taken from: https://stackoverflow.com/a/52208842

    template<unsigned i, typename Item>
    struct TupleLeaf {
      Item value;
    };

    template<unsigned i, typename... Items>
    struct TupleImpl;

    // Base case: empty tuple
    template<unsigned i>
    struct TupleImpl<i>{};

    // Recursive specialization
    template<unsigned i, typename HeadItem, typename... TailItems>
    struct TupleImpl<i, HeadItem, TailItems...> :
        public TupleLeaf<i, HeadItem>,
        public TupleImpl<i + 1, TailItems...>
        {};

    template<unsigned i, typename HeadItem, typename... TailItems>
    HeadItem& Get(TupleImpl<i, HeadItem, TailItems...>& tuple) {
        return tuple.TupleLeaf<i, HeadItem>::value;
    }

    template<unsigned i, typename HeadItem, typename... TailItems>
    void Store(TupleImpl<i, HeadItem, TailItems...>& tuple,
        const HeadItem first, const TailItems ...tail) {
        tuple.TupleLeaf<i, HeadItem>::value = first;
        Store(static_cast<TupleImpl<i + 1, TailItems...> &>(tuple), tail...);
    }

    template<unsigned i>
    void Store(TupleImpl<i> &)
    { }

    template<unsigned i, typename HeadItem, typename... TailItems>
    void Load(const TupleImpl<i, HeadItem, TailItems...>& tuple,
        HeadItem &first, TailItems& ...tail) {
        first = tuple.TupleLeaf<i, HeadItem>::value;
        Load(static_cast<const TupleImpl<i + 1, TailItems...> &>(tuple),
            tail...);
    }

    template<unsigned i>
    void Load(const TupleImpl<i> &)
    { }
#   endif /// Custom std::tuple<>

    TupleImpl<0, Types...> _values;

  public:
    void raise(const Types... vals) {
      auto l = SignalType::lock();

      Store(_values, vals...);
      SignalType::raise();
    }

    bool query(Types& ...outs) {
      if (!SignalType::query())
        return false;

      auto l = SignalType::lock();

      Load(_values, outs...);
      return true;
    }
  };

  template <typename SignalType>
  class BaseMessage<SignalType> : public SignalType
  { }; // Empty message is just a signal, this saves a byte

  template <typename ...Types>
  using Message = BaseMessage<Signal, Types...>;

  template <typename ...Types>
  using UpdatedMessage = BaseMessage<UpdatedSignal, Types...>;
}
