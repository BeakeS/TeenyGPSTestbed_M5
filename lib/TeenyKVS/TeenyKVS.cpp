/*  TeenyKVS - KeyValueStore Server

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "TeenyKVS.h"


/********************************************************************/
// TeenyKVS class
/********************************************************************/
TeenyKVS::TeenyKVS(uint16_t size, uint16_t startAddr,
                   uint8_t (*fetch)(uint16_t addr),
                   bool (*store)(uint16_t addr, uint8_t value),
                   void (*commit)(),
                   bool invertStorage) :
  kvsSize(size),
  kvsStartAddr(startAddr),
  kvsFetch(fetch),
  kvsStore(store),
  kvsCommit(commit),
  kvsInvertStorage(invertStorage)
{
}

TeenyKVS::~TeenyKVS() {
}

/********************************************************************/
bool TeenyKVS::reset() {
  return kvs_reset();
}

bool TeenyKVS::check() {
  return kvs_trace();
}

/********************************************************************/
bool TeenyKVS::set(const uint8_t *key, uint16_t key_len, const uint8_t *value, uint16_t value_len) {
  return kvs_set(key, key_len, value, value_len);
}
bool TeenyKVS::set(const char *key, uint16_t key_len, const uint8_t *value, uint16_t value_len) {
  return kvs_set((uint8_t*)key, key_len, value, value_len);
}
bool TeenyKVS::set(const char *key, uint16_t key_len, const char *value, uint16_t value_len) {
  return kvs_set((uint8_t*)key, key_len, (uint8_t*)value, value_len);
}

bool TeenyKVS::find(const uint16_t key_num, uint16_t *key_len, uint16_t *value_len) {
  uint16_t _kvsKeyPos, _kvsValuePos;
  return kvs_find(key_num, key_len, &_kvsKeyPos, value_len, &_kvsValuePos);
}

bool TeenyKVS::find(const uint8_t *key, uint16_t key_len, uint16_t *value_len) {
  uint16_t _kvsKeyPos, _kvsValuePos;
  return kvs_find(key, key_len, &_kvsKeyPos, value_len, &_kvsValuePos);
}
bool TeenyKVS::find(const char *key, uint16_t key_len, uint16_t *value_len) {
  uint16_t _kvsKeyPos, _kvsValuePos;
  return kvs_find((uint8_t*)key, key_len, &_kvsKeyPos, value_len, &_kvsValuePos);
}

bool TeenyKVS::get(const uint8_t *key, uint16_t key_len, uint8_t *value, uint16_t value_len) {
  return kvs_get(key, key_len, value, value_len);
}
bool TeenyKVS::get(const char *key, uint16_t key_len, uint8_t *value, uint16_t value_len) {
  return kvs_get((uint8_t*)key, key_len, value, value_len);
}
bool TeenyKVS::get(const char *key, uint16_t key_len, char *value, uint16_t value_len) {
  return kvs_get((uint8_t*)key, key_len, (uint8_t*)value, value_len);
}

bool TeenyKVS::del(const uint8_t *key, uint16_t key_len) {
  return kvs_del(key, key_len);
}
bool TeenyKVS::del(const char *key, uint16_t key_len) {
  return kvs_del((uint8_t*)key, key_len);
}

/********************************************************************/
uint16_t TeenyKVS::used_bytes() {
  kvs_trace();
  return kvsUsedBytes;
}

uint16_t TeenyKVS::free_bytes() {
  return kvsSize - used_bytes();
}

uint16_t TeenyKVS::num_keys() {
  kvs_trace();
  return kvsNumKeys;
}

/********************************************************************/
bool TeenyKVS::kvs_trace() {
  return kvs_exec(KVSEXEC_TRACE, 0, nullptr, 0, nullptr, nullptr, nullptr, nullptr);
}

bool TeenyKVS::kvs_find(const uint16_t key_num, uint16_t *key_len, uint16_t *key_pos, uint16_t *value_len, uint16_t *value_pos) {
  return kvs_exec(KVSEXEC_GETKEYNUM, key_num, nullptr, key_len, key_pos, nullptr, value_len, value_pos);
}

bool TeenyKVS::kvs_find(const uint8_t *key, uint16_t key_len, uint16_t *key_pos, uint16_t *value_len, uint16_t *value_pos) {
  return kvs_exec(KVSEXEC_GET, 0, key, &key_len, key_pos, nullptr, value_len, value_pos);
}

bool TeenyKVS::kvs_get(const uint8_t *key, uint16_t key_len,
                       uint8_t *value, uint16_t value_len) {
  uint16_t _kvsKeyPos, _kvsValueLen, _kvsValuePos;
  if(kvs_exec(KVSEXEC_GET, 0, key, &key_len, &_kvsKeyPos, nullptr, &_kvsValueLen, &_kvsValuePos)) {
    if(value_len==_kvsValueLen) {
      for(uint16_t i = 0; i < _kvsValueLen; i++) {
        value[i] = kvs_fetch(_kvsValuePos+i);
      }
      return true;
    }
  }
  return false;
}

bool TeenyKVS::kvs_set(const uint8_t *key, uint16_t key_len,
                       const uint8_t *value, uint16_t value_len) {
  if(key_len==0) return false;
  uint16_t _kvsKeyPos, _kvsValueLen, _kvsValuePos;
  bool _isFound=false;
  if(kvs_exec(KVSEXEC_GET, 0, key, &key_len, &_kvsKeyPos, nullptr, &_kvsValueLen, &_kvsValuePos)) {
    _isFound = true;
  }
  if(_isFound && (value_len==_kvsValueLen)) {
    // overwrite value when value length matches
    return kvs_exec(KVSEXEC_SET, 0, key, &key_len, &_kvsKeyPos, value, &value_len, &_kvsValuePos);
  }
  if(_isFound && (value_len<_kvsValueLen)) {
    // delete existing and create new at tail
    return kvs_exec(KVSEXEC_SET, 0, key, &key_len, &_kvsKeyPos, value, &value_len, &_kvsValuePos);
  }
  // Compute free space
  uint16_t _kvsFreeBytes = free_bytes();
  if(_isFound) {
    if(_kvsFreeBytes>=(value_len-_kvsValueLen)) {
      // delete existing and create new at tail
      return kvs_exec(KVSEXEC_SET, 0, key, &key_len, &_kvsKeyPos, value, &value_len, &_kvsValuePos);
    }
    return false;
  }
  if(_kvsFreeBytes>=(2+key_len+2+value_len)) {
    // create new at tail
    return kvs_exec(KVSEXEC_SET, 0, key, &key_len, &_kvsKeyPos, value, &value_len, &_kvsValuePos);
  }
  return false;
}

bool TeenyKVS::kvs_del(const uint8_t *key, uint16_t key_len) {
  return kvs_exec(KVSEXEC_DEL, 0, key, &key_len, nullptr, nullptr, nullptr, nullptr);
}

/********************************************************************/
bool TeenyKVS::kvs_reset() {
  static bool reset_lock = false;
  bool rc1, rc2;
  if(!reset_lock) {
    reset_lock = true;
    rc1 = kvs_store(0, 0);
    rc2 = kvs_store(1, 0);
    kvsUsedBytes = 2;
    kvsNumKeys = 0;
    reset_lock = false;
    return rc1 && rc2;
  }
  return false;
}

/********************************************************************/
bool TeenyKVS::kvs_exec(kvsExecMode_t mode, const uint16_t key_num, const uint8_t *key_name, uint16_t *key_len, uint16_t *key_pos, const uint8_t *value, uint16_t *value_len, uint16_t *value_pos) {
  uint16_t _kvsPtr=0, _kvsNumKeys=0, _deleteOffset=0;
  uint16_t _kvsKeyPtr, _kvsKeyLen, _kvsKeyPos, _kvsValPtr, _kvsValLen, _kvsValPos;
  bool _foundKey=false;
  typedef enum {AT_KEY_LEN, AT_KEY_POS, AT_VAL_LEN, AT_VAL_POS} _kvsPtrLoc_t;
  _kvsPtrLoc_t _kvsPtrLoc=AT_KEY_LEN;
  while(_kvsPtr < kvsSize-2) {
    // Process KEY len/pos
    if(_kvsPtrLoc==AT_KEY_LEN) {
      _kvsKeyPtr = _kvsPtr;
      _kvsKeyLen = (uint16_t)((uint8_t)kvs_fetch(_kvsPtr) + (kvs_fetch(_kvsPtr+1) << 8));
      if((_kvsKeyLen+6) >= (kvsSize-_kvsKeyPtr)) {
        kvs_reset();
        return false;
      }
      if(_kvsKeyLen==0) {
        // end tag (key length = 0)
        break;
      }
      _kvsNumKeys++;
      if(_deleteOffset) {
        kvs_store(_kvsPtr-_deleteOffset, kvs_fetch(_kvsPtr));
        kvs_store((_kvsPtr+1)-_deleteOffset, kvs_fetch(_kvsPtr+1));
      }
      _kvsPtr += 2;
      _kvsKeyPos = _kvsPtr;
      _kvsPtrLoc = AT_KEY_POS;
      if(_deleteOffset) {
        for(uint16_t i=0; i<_kvsKeyLen; i++) {
          kvs_store((_kvsPtr+i)-_deleteOffset, kvs_fetch(_kvsPtr+i));
        }
      } else if(mode==KVSEXEC_GET || mode==KVSEXEC_SET || mode==KVSEXEC_DEL) {
        _foundKey = false;
        if(_kvsKeyLen==*key_len) {
          _foundKey = true;
          for(uint16_t i=0; i<_kvsKeyLen; i++) {
            if(kvs_fetch(_kvsPtr+i) != key_name[i]) {
              _foundKey = false;
              break;
            }
          }
        }
      }
      _kvsPtr += _kvsKeyLen;
      _kvsValPtr = _kvsPtr;
      _kvsPtrLoc = AT_VAL_LEN;
    }
    // Process VALUE len/pos
    if(_kvsPtrLoc!=AT_VAL_LEN) {
      kvs_reset();
      return false;
    }
    _kvsValLen = (uint16_t)((uint8_t)kvs_fetch(_kvsPtr) + (kvs_fetch(_kvsPtr+1) << 8));
    if((_kvsValLen+4) >= (kvsSize-_kvsPtr)) {
      kvs_reset();
      return false;
    }
    if(_deleteOffset) {
      kvs_store(_kvsPtr-_deleteOffset, kvs_fetch(_kvsPtr));
      kvs_store((_kvsPtr+1)-_deleteOffset, kvs_fetch(_kvsPtr+1));
    }
    _kvsPtr += 2;
    _kvsValPos = _kvsPtr;
    _kvsPtrLoc = AT_VAL_POS;
    if(_deleteOffset) {
      for(uint16_t i=0; i<_kvsValLen; i++) {
        kvs_store((_kvsPtr+i)-_deleteOffset, kvs_fetch(_kvsPtr+i));
      }
    }
    // Process key number for GETKEYNUM
    if(mode==KVSEXEC_GETKEYNUM && _kvsNumKeys==key_num) {
      *key_len   = _kvsKeyLen;
      *key_pos   = _kvsKeyPos;
      *value_len = _kvsValLen;
      *value_pos = _kvsValPos;
      return true;
    }
    // Process found key for GET, SET, or DEL
    if(_foundKey) {
      if(mode==KVSEXEC_GET) {
        // GET when key matches
        *key_pos   = _kvsKeyPos;
        *value_len = _kvsValLen;
        *value_pos = _kvsValPos;
        return true;
      }
      if(mode==KVSEXEC_SET && *value_len==_kvsValLen) {
        // overwrite value when key matches and same value length
        for(uint16_t i=0; i<_kvsValLen; i++) {
          kvs_store(_kvsPtr+i, value[i]);
        }
        if(kvsCommit) (kvsCommit)();
        return true;
      }
      if(mode==KVSEXEC_SET || mode==KVSEXEC_DEL) {
        // SET or DEL when key matches
        _deleteOffset = (_kvsPtr + _kvsValLen) - _kvsKeyPtr;
      }
      _foundKey = false;
    }
    _kvsPtr += _kvsValLen;
    _kvsPtrLoc = AT_KEY_LEN;
  }
  // Finish processsing SET, DEL, and TRACE
  if(_kvsPtrLoc!=AT_KEY_LEN) {
    kvs_reset();
    return false;
  }
  if(mode==KVSEXEC_SET) {
    // SET - store new key value at end of kvs
    _kvsPtr -= _deleteOffset;
    kvs_store(_kvsPtr, *key_len & 0xFF);
    kvs_store(_kvsPtr+1, (*key_len>>8) & 0xFF);
    _kvsPtr += 2;
    _kvsPtrLoc = AT_KEY_POS;
    for(uint16_t i=0; i<*key_len; i++) {
      kvs_store(_kvsPtr+i, key_name[i]);
    }
    _kvsPtr += *key_len;
    _kvsPtrLoc = AT_VAL_LEN;
    kvs_store(_kvsPtr, *value_len & 0xFF);
    kvs_store(_kvsPtr+1, (*value_len>>8) & 0xFF);
    _kvsPtr += 2;
    _kvsPtrLoc = AT_VAL_POS;
    for(uint16_t i=0; i<*value_len; i++) {
      kvs_store(_kvsPtr+i, value[i]);
    }
    _kvsPtr += *value_len;
    _kvsPtrLoc = AT_KEY_LEN;
    // SET - add end tag (key length = 0)
    kvs_store(_kvsPtr, 0);
    kvs_store(_kvsPtr+1, 0);
    if(kvsCommit) (kvsCommit)();
    return true;
  }
  if(mode==KVSEXEC_DEL && _deleteOffset) {
    // DEL - put end tag (key length = 0)
    _kvsPtr -= _deleteOffset;
    kvs_store(_kvsPtr, 0);
    kvs_store(_kvsPtr+1, 0);
    if(kvsCommit) (kvsCommit)();
    return true;
  }
  if(mode==KVSEXEC_TRACE) {
    kvsUsedBytes = _kvsPtr + 2;
    kvsNumKeys = _kvsNumKeys;
    return true;
  }
  return false;
}

/********************************************************************/
uint8_t TeenyKVS::kvs_fetch(uint16_t addr) {
  if(kvsInvertStorage) {
    return (kvsFetch)(((kvsSize-1)+kvsStartAddr)-addr);
  }
  return (kvsFetch)(kvsStartAddr+addr);
}

/********************************************************************/
bool TeenyKVS::kvs_store(uint16_t addr, uint8_t value) {
  if(kvsInvertStorage) {
    return (kvsStore)(((kvsSize-1)+kvsStartAddr)-addr, value);
  }
  return (kvsStore)(kvsStartAddr+addr, value);
}


/********************************************************************/
// TeenyKVSArray class
/********************************************************************/
TeenyKVSArray::TeenyKVSArray(uint8_t *teenyKVSArray, uint16_t teenyKVSArraySize) :
  _teenyKVSArray(teenyKVSArray),
  TeenyKVS(teenyKVSArraySize)
{
}

/********************************************************************/
uint8_t TeenyKVSArray::kvs_fetch(uint16_t addr) {
  return _teenyKVSArray[addr];
}

bool TeenyKVSArray::kvs_store(uint16_t addr, uint8_t value) {
  _teenyKVSArray[addr] = value;
  return true;
}

