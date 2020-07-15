// File: cachealgs.h
/*
    Pep9 is a virtual machine for writing machine language and assembly
    language programs.

    Copyright (C) 2020  Matthew McRaven & J. Stanley Warford, Pepperdine University

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
#ifndef LRUREPLACE_H
#define LRUREPLACE_H

#include <random>
#include <deque>

#include <QMetaObject>
#include <QMetaEnum>
#include <QVector>
#include <QSharedPointer>

#include "cachereplace.h"

/*
 * Algorithms Summary
 */
namespace CacheAlgorithms{
    Q_NAMESPACE
    enum CacheAlgorithms {
        // Count
        LRU, MRU,
        BPLRU,
        LFU, LFUDA, MFU,
        FIFO,
        Random,
    };
    Q_ENUM_NS(CacheAlgorithms);
    static QMap<CacheAlgorithms, AReplacementFactory*> factory;
}
QSharedPointer<AReplacementFactory> getPolicyFactory(CacheAlgorithms::CacheAlgorithms algorithm, quint16 size);
/*
 * Least/Most Recently Used (LRU/MRU) Replacement
 */

class RecentReplace : public AReplacementPolicy
{

protected:
    // Fully-renumber all entries after count accesses have elapsed.
    // Necessary to prevent overflow on count. Re-numbering may be expensive,
    // so perform the operation very rarely.
    static const quint32 renumber_threshold = 0x10'000;
    quint32 count;
    int index_last;
    QVector<std::tuple<bool, quint32>> last_access;

public:
    typedef QVector<std::tuple<bool, quint32>>::const_iterator iterator;
    typedef std::function<iterator(iterator, iterator)> SelectFunction;
public:
    RecentReplace(quint16 size, SelectFunction element_select);
    virtual ~RecentReplace() override = 0 ;

    bool canAge() const override { return true;}
    // For derived replacement algorithms, the magnitude of the difference in access times
    // is considered to be irrelevant. If the magnitude of age differences is important,
    // rather than just ordering, override it.
    void age() override;


    // AReplacementPolicy interface
public:
    void reference(quint16 index) override;
    quint16 evict() override;
    quint16 supports_evicition_lookahead() const override;
    quint16 eviction_loohahead() const override;
    QVector<quint16> eviction_loohahead(quint16 count) const override;
    void clear() override;

protected:
   SelectFunction element_select;
};

class LRUReplace : public RecentReplace
{
public:
    LRUReplace(quint16 size);
    virtual ~LRUReplace() override = default;

    QString get_algorithm_name() const override;
};

class MRUReplace : public RecentReplace
{
public:
    MRUReplace(quint16 size);
    virtual ~MRUReplace() override = default;

    QString get_algorithm_name() const override;
};

class LRUFactory : public AReplacementFactory
{
public:
    LRUFactory(quint16 associativity);
    ~LRUFactory() override = default;

    QSharedPointer<AReplacementPolicy> create_policy() override;
    QString get_algorithm_name() const override {return algorithm();}
    static const QString algorithm();
    static CacheAlgorithms::CacheAlgorithms algorithm_enum();
};

class MRUFactory : public AReplacementFactory
{
public:
    MRUFactory(quint16 associativity);
    ~MRUFactory() override = default;

    QSharedPointer<AReplacementPolicy> create_policy() override;
    QString get_algorithm_name() const override {return algorithm();}
    static const QString algorithm();
    static CacheAlgorithms::CacheAlgorithms algorithm_enum();
};

/*
 * Least/Most Frequently Used (LFU/MFU) Replacement
 */

class FrequencyReplace : public AReplacementPolicy
{

protected:
    QVector<quint32> access_count;

public:
    typedef QVector<quint32>::const_iterator iterator;
    typedef std::function<iterator(iterator, iterator)> SelectFunction;
    typedef std::function<quint32(iterator,iterator)> InitFunction;
public:
    // Init function determines what the initial value for an item is.
    // It is given the begin and end iterators for access count, and computes the starting value
    // over the access count array.
    FrequencyReplace(quint16 size, SelectFunction element_select, InitFunction init_function);
    virtual ~FrequencyReplace() override = 0 ;


    // AReplacementPolicy interface
public:
    void reference(quint16 index) override;
    quint16 evict() override;
    quint16 supports_evicition_lookahead() const override;
    quint16 eviction_loohahead() const override;
    QVector<quint16> eviction_loohahead(quint16 count) const override;
    void clear() override;

private:
   SelectFunction element_select;
   InitFunction init_function;
};

class LFUReplace : public FrequencyReplace
{
public:
    LFUReplace(quint16 size);
    virtual ~LFUReplace() override = default;

    QString get_algorithm_name() const override;
};

class LFUDAReplace : public FrequencyReplace
{
public:
   LFUDAReplace(quint16 size, quint16 age_steps=10);
   virtual ~LFUDAReplace() override = default;
   QString get_algorithm_name() const override;
   bool canAge() const override { return true;}
   void age() override;
private:
   int timer, age_after;
};

class MFUReplace : public FrequencyReplace
{
public:
    MFUReplace(quint16 size);
    virtual ~MFUReplace() override = default;

    QString get_algorithm_name() const override;
};

class LFUFactory : public AReplacementFactory
{
public:
    LFUFactory(quint16 associativity);
    ~LFUFactory() override = default;

    QSharedPointer<AReplacementPolicy> create_policy() override;
    QString get_algorithm_name() const override {return algorithm();}
    static const QString algorithm();
    static CacheAlgorithms::CacheAlgorithms algorithm_enum();
};

class LFUDAFactory : public AReplacementFactory
{
public:
    LFUDAFactory(quint16 associativity, quint16 age_after=10);
    ~LFUDAFactory() override = default;

    QSharedPointer<AReplacementPolicy> create_policy() override;
    QString get_algorithm_name() const override {return algorithm();}
    static const QString algorithm();
    static CacheAlgorithms::CacheAlgorithms algorithm_enum();
private:
    quint16 age_after;
};

class MFUFactory : public AReplacementFactory
{
public:
    MFUFactory(quint16 associativity);
    ~MFUFactory() override = default;

    QSharedPointer<AReplacementPolicy> create_policy() override;
    QString get_algorithm_name() const override {return algorithm();}
    static const QString algorithm();
    static CacheAlgorithms::CacheAlgorithms algorithm_enum();
};

/*
 * Order based replacement algorithms.
 */

class FIFOReplace : public AReplacementPolicy
{

protected:
    quint16 size, next_victim;
public:
    FIFOReplace(quint16 size);
    virtual ~FIFOReplace() override = default;

    // AReplacementPolicy interface
public:
    void reference(quint16 index) override;
    QString get_algorithm_name() const override;
    quint16 evict() override;
    quint16 supports_evicition_lookahead() const override;
    quint16 eviction_loohahead() const override;
    QVector<quint16> eviction_loohahead(quint16 count) const override;
    void clear() override;
};

class FIFOFactory : public AReplacementFactory
{
public:
    FIFOFactory(quint16 associativity);
    ~FIFOFactory() override = default;

    QSharedPointer<AReplacementPolicy> create_policy() override;
    QString get_algorithm_name() const override {return algorithm();}
    static const QString algorithm();
    static CacheAlgorithms::CacheAlgorithms algorithm_enum();
};

/*
 * Random Replacement
 */
class RandomReplace : public AReplacementPolicy
{
public:
    RandomReplace(std::function<quint16()> get_random);
    virtual ~RandomReplace() override = default;

    // AReplacementPolicy interface
public:
    void reference(quint16 index) override;
    QString get_algorithm_name() const override;
    quint16 evict() override;
    quint16 supports_evicition_lookahead() const override;
    quint16 eviction_loohahead() const override;
    QVector<quint16> eviction_loohahead(quint16 count) const override;
    void clear() override;

private:
    // Support a single look-ahead for eviction.
    mutable bool has_next_random = false;
    mutable quint16 next_random = 0;
    std::function<quint16()> get_random;

};

class RandomFactory : public AReplacementFactory
{
public:
    RandomFactory(quint16 associativity);
    ~RandomFactory() override = default;

    QSharedPointer<AReplacementPolicy> create_policy() override;
    QString get_algorithm_name() const override;
    static const QString algorithm();
    static CacheAlgorithms::CacheAlgorithms algorithm_enum();
private:
    std::default_random_engine generator{};
    std::uniform_int_distribution<quint16> distribution;
    std::function<quint16()> random_function;
};

/*
 * Pseudo-LRU
 */

// Implements the 1-bit Pseudo-LRU replacement policy.
// It "remebers" which half of the cache is msot recently used,
// and protects the entries on that side.
// A random cache entry from the unprotected side is evicted.
// See description, available here:
//      https://people.kth.se/~ingo/MasterThesis/ThesisDamienGille2007.pdf
class BPLRU : public AReplacementPolicy
{
public:
    BPLRU(quint16 associativity, std::function<quint16()>& evict_left, std::function<quint16()> evict_right);
    virtual ~BPLRU() override = default;

    // AReplacementPolicy interface
public:
    void reference(quint16 index) override;
    QString get_algorithm_name() const override;
    quint16 evict() override;
    quint16 supports_evicition_lookahead() const override;
    quint16 eviction_loohahead() const override;
    QVector<quint16> eviction_loohahead(quint16 count) const override;
    void clear() override;
protected:
    quint16 associativity;

    // Support a single look-ahead for eviction.
    mutable bool has_next_random = false;
    mutable quint16 next_random = 0;

    // Deterimine which side to random evict from.
    enum class MRUSide {
        RIGHT, LEFT
    };
    MRUSide side = MRUSide::LEFT;
    std::function<quint16()>& evict_left, evict_right;
};
class BPLRUFactory : public AReplacementFactory
{
public:
    BPLRUFactory(quint16 associativity);
    ~BPLRUFactory() override = default;

    QSharedPointer<AReplacementPolicy> create_policy() override;
    QString get_algorithm_name() const override;
    static const QString algorithm();
    static CacheAlgorithms::CacheAlgorithms algorithm_enum();

private:
    std::default_random_engine generator{};
    std::uniform_int_distribution<quint16> evict_left, evict_right;
    std::function<quint16()> rand_left, rand_right;
};

#endif // LRUREPLACE_H