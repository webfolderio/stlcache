//
// Copyright (C) 2011 Denis V Chapligin
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef STLCACHE_POLICY_LFUAGINGSTAR_HPP_INCLUDED
#define STLCACHE_POLICY_LFUAGINGSTAR_HPP_INCLUDED

#include <set>
#include <map>
#include <ctime>

using namespace std;

#include <stlcache/policy.hpp>
#include <stlcache/policy_lfuaging.hpp>
#include <stlcache/policy_lfustar.hpp>

#ifdef _MSC_VER
#pragma warning( disable : 4250 )
#endif /* _MSC_VER */

namespace stlcache {
    template <class Key,time_t Age=3600> class policy_lfuagingstar : public virtual policy_lfuaging<Key,Age>, virtual policy_lfustar<Key> {
    public:
        policy_lfuagingstar(const size_t& size ) throw() : policy_lfuaging<Key,Age>(size), policy_lfustar<Key>(size),policy_lfu<Key>(size) { }

        virtual const _victim<Key> victim() throw()  {
			policy_lfuaging<Key,Age>::expire();
            return policy_lfustar<Key>::victim();
        }
    };
}

#endif /* STLCACHE_POLICY_LFUAGINGSTAR_HPP_INCLUDED */