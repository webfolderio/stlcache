//
// Copyright (C) 2011 Denis V Chapligin
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef STLCACHE_POLICY_LFU_HPP_INCLUDED
#define STLCACHE_POLICY_LFU_HPP_INCLUDED

#include <set>
#include <map>

using namespace std;

#include <stlcache/policy.hpp>

namespace stlcache {
    template <class Key> class policy_lfu : public policy<Key> {
        typedef set<Key> keySet;
        map<unsigned long long,keySet > _entries;
        map<Key, unsigned long long> _backEntries;
    public:
        policy_lfu<Key>& operator= ( const policy_lfu<Key>& x) throw() {
            this->_entries=x._entries;
            this->_backEntries=x._backEntries;
            return *this;
        }
        policy_lfu(const policy_lfu<Key>& x) throw() {
            *this=x;
        }
        policy_lfu(const size_t& size ) throw() { }

        virtual void insert(const Key& _k) throw(stlcache_invalid_key) {
            //1 - is initial reference value
            keySet pad=_entries[1];
            pad.insert(_k);
            if (!_backEntries.insert(std::pair<Key,unsigned long long>(_k,1)).second) {
                throw stlcache_invalid_key("Key already cached!",_k);
            }

            _entries.erase(1);
            _entries.insert(std::pair<unsigned long long, keySet>(1,pad));
        }
        virtual void remove(const Key& _k) throw() {
            keySet pad = _entries[_backEntries[_k]];

            pad.erase(_k);
            _entries.erase(_backEntries[_k]);
            if (pad.size()>0) {
                _entries.insert(std::pair<unsigned long long, keySet>(_backEntries[_k],pad));
            }

            _backEntries.erase(_k);
        }
        virtual void touch(const Key& _k) throw() { 
            unsigned long long ref = _backEntries[_k];

            keySet pad = _entries[ref];
            pad.erase(_k);
            _entries.erase(ref);
			if (!pad.empty()) {
				_entries.insert(std::pair<unsigned long long, keySet>(ref,pad));
			}

            ref++;
			if (_entries.find(ref)!=_entries.end()) {
				pad = _entries[ref];
			} else {
				pad = keySet();
			}
            pad.insert(_k);
            _entries.erase(ref);
            _entries.insert(std::pair<unsigned long long, keySet>(ref,pad));

            _backEntries.erase(_k);
            _backEntries[_k]=ref;
        }
        virtual void clear() throw() {
            _entries.clear();
            _backEntries.clear();
        }
        virtual void swap(policy<Key>& _p) throw(stlcache_invalid_policy) {
            try {
                policy_lfu<Key>& _pn=dynamic_cast<policy_lfu<Key>& >(_p);
                _entries.swap(_pn._entries);
                _backEntries.swap(_pn._backEntries);
            } catch (const std::bad_cast& ) {
                throw stlcache_invalid_policy("Attempted to swap incompatible policies");
            }
        }

        virtual const _victim<Key> victim() throw()  {
            if (_entries.begin()==_entries.end()) {
                return _victim<Key>();
            }
            keySet pad=(*(_entries.begin())).second; //Begin returns entry with lowest id, just what we need:)

            if (pad.begin()==pad.end()) {
                return _victim<Key>();
            }

            return _victim<Key>(*(pad.begin()));
        }

    protected:
        const map<unsigned long long,keySet >& entries() const {
            return this->_entries;
        }
        virtual unsigned long long untouch(const Key& _k) throw() { 
            unsigned long long ref = _backEntries[_k];

            if (!(ref>1)) {
                return ref; //1 is a minimal reference value
            }

            keySet pad = _entries[ref];
            pad.erase(_k);
            _entries.erase(ref);
            _entries.insert(std::pair<unsigned long long, keySet>(ref,pad));

            ref--;
            pad = _entries[ref];
            pad.insert(_k);
            _entries.erase(ref);
            _entries.insert(std::pair<unsigned long long, keySet>(ref,pad));

            _backEntries.erase(_k);
            _backEntries[_k]=ref;

            return ref;
        }
    };
}

#endif /* STLCACHE_POLICY_LFU_HPP_INCLUDED */