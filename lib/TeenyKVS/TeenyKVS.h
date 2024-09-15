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

#ifndef TEENYKVS_H
#define TEENYKVS_H

#include "Arduino.h"


/********************************************************************/
// TeenyKVS class
/********************************************************************/
class TeenyKVS {

  public:

    // Constructor / destructor / disallow copy and move
    TeenyKVS(uint16_t size=2, uint16_t startAddr=0,
             uint8_t (*fetch)(uint16_t addr)=nullptr,
             bool (*store)(uint16_t addr, uint8_t value)=nullptr,
             void (*commit)()=nullptr,
             bool invertStorage=false);
    ~TeenyKVS();
    TeenyKVS(const TeenyKVS&);
    TeenyKVS& operator=(const TeenyKVS&);

    // required
    uint8_t (*kvsFetch)(uint16_t addr);
    bool (*kvsStore)(uint16_t addr, uint8_t value);
    // optional
    void (*kvsCommit)();

    // KVS access
    virtual bool reset();
    bool check();
    bool set(const uint8_t *key, uint16_t key_len, const uint8_t *value, uint16_t value_len);
    bool set(const char *key, uint16_t key_len, const uint8_t *value, uint16_t value_len);
    bool set(const char *key, uint16_t key_len, const char *value, uint16_t value_len);
    bool find(const uint16_t key_num, uint16_t *key_len, uint16_t *value_len);
    bool find(const uint8_t *key, uint16_t key_len, uint16_t *value_len);
    bool find(const char *key, uint16_t key_len, uint16_t *value_len);
    bool get(const uint8_t *key, uint16_t key_len, uint8_t *value, uint16_t value_len);
    bool get(const char *key, uint16_t key_len, uint8_t *value, uint16_t value_len);
    bool get(const char *key, uint16_t key_len, char *value, uint16_t value_len);
    bool del(const uint8_t *key, uint16_t key_len);
    bool del(const char *key, uint16_t key_len);

    // KVS utils
    uint16_t used_bytes();
    uint16_t free_bytes();
    uint16_t num_keys();

  protected:

    bool kvs_reset();

  private:

    uint16_t kvsSize, kvsStartAddr, kvsNumKeys, kvsUsedBytes;
    bool kvsInvertStorage;

    bool kvs_trace();
    bool kvs_find(const uint16_t key_num, uint16_t *key_len, uint16_t *key_pos,
                  uint16_t *value_len, uint16_t *value_pos);
    bool kvs_find(const uint8_t *key, uint16_t key_len, uint16_t *key_pos,
                  uint16_t *value_len, uint16_t *value_pos);
    bool kvs_get(const uint8_t *key, uint16_t key_len,
                 uint8_t *value, uint16_t value_len);
    bool kvs_set(const uint8_t *key, uint16_t key_len,
                 const uint8_t *value, uint16_t value_len);
    bool kvs_del(const uint8_t *key, uint16_t key_len);

    typedef enum {
        KVSEXEC_TRACE = 0,
        KVSEXEC_GETKEYNUM,
        KVSEXEC_GET,
        KVSEXEC_SET,
        KVSEXEC_DEL
    } kvsExecMode_t;

    bool kvs_exec(kvsExecMode_t mode,
                  const uint16_t key_num,
                  const uint8_t *key_name, uint16_t *key_len, uint16_t *key_pos,
                  const uint8_t *value, uint16_t *value_len, uint16_t *value_pos);
    
    virtual uint8_t kvs_fetch(uint16_t addr);
    virtual bool    kvs_store(uint16_t addr, uint8_t value);

};


/********************************************************************/
// TeenyKVSArray class
/********************************************************************/
class TeenyKVSArray : public TeenyKVS {

  public:

    TeenyKVSArray(uint8_t *teenyKVSArray, uint16_t teenyKVSArraySize);

  protected:

  private:

    uint8_t  *_teenyKVSArray;
    uint8_t  kvs_fetch(uint16_t addr);
    bool     kvs_store(uint16_t addr, uint8_t value);

};

#endif //TEENYKVS_H

