// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"

#include "random.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

using namespace std;
using namespace boost::assign;

struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};

#include "chainparamsseeds.h"

//
// Main network
//

// Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress> &vSeedsOut, const SeedSpec6 *data, unsigned int count)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7*24*60*60;
    for (unsigned int i = 0; i < count; i++)
    {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
static Checkpoints::MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
 	// before block 14640, the retarget period in Devcoin was 1 week
	// the function ComputeMinWork uses the current retarget period
	// of 3 hours, this is no problem UNLESS someone adds a checkpoint before
	// block 14640, should be no problem....
	( 2500, uint256("0x000000001871a2314936d39b85174cc911bf6fd58d3877412ee7b69a48e7e29e"))
	( 4500, uint256("0x000000000967cc95711f66f804e3f431298686d681d2d5760f61856954d08faf"))
	( 5250, uint256("0x00000000085702bfbf27daffb638be65aceb78a5f464b12539b51c1b9c548421"))
	( 8900, uint256("0x00000000001bb8090630fcabb82ad0ab75df3eb5b008956b3ae2a352a4324f19"))
	( 23500, uint256("0x000000000b83c3c9753d2440b91121cb0ff220bb23c136c6d09a539207e292fb"))
	( 54800, uint256("0x04e8dcc91ff2aa0f1197f88551b4cb24ccef02ea51081b4d05ab4e3a38554137"))
	( 67720, uint256("0x0a111b265d89f77b4c86fa6f44e3e2ad876547b1eccf19319cde922b42c1161e"))	;
static const Checkpoints::CCheckpointData data = {
        &mapCheckpoints,
        1354231672, // * UNIX timestamp of last checkpoint block
        936858,     // * total number of transactions between genesis and last checkpoint
                    //   (the tx=... number in the SetBestChain debug.log lines)
        1000.0      // * estimated number of transactions per day after checkpoint
    };
    /* Devcoin had not testnet checkblocks, if they even start to exist, they can be added here */
    static Checkpoints::MapCheckpoints mapCheckpointsTestnet  = boost::assign::map_list_of 
		(0,  uint256("0x00000000fc09a99bd5116e9cedcad35d2145962799e58bbfd66ebdeb4e95235f"));
static const Checkpoints::CCheckpointData dataTestnet = {
        &mapCheckpointsTestnet,
        1387426393,
        1,
        1
    };

static Checkpoints::MapCheckpoints mapCheckpointsRegtest =
        boost::assign::map_list_of
        ( 0, uint256("0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206"))
        ;
static const Checkpoints::CCheckpointData dataRegtest = {
        &mapCheckpointsRegtest,
        0,
        0,
        0
    };

class CMainParams : public CChainParams {
public:
    CMainParams() {
        networkID = CBaseChainParams::MAIN;
        strNetworkID = "main";
        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[0] = 'D';
        pchMessageStart[1] = 'E';
        pchMessageStart[2] = 'V';
		pchMessageStart[3] = ':';
        vAlertPubKey = ParseHex("04fc9702847840aaf195de8442ebecedf5b095cdbb9bc716bda9110971b28a49e0ead8564ff0db22209e0374782c093bb899692d524e9d6a6956e7c5ecbcd68284");
        nDefaultPort = 52333;
        bnProofOfWorkLimit = ~uint256(0) >> 32;
        nSubsidyHalvingInterval = 210000;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 0;
        nTargetTimespan = 24 * 60 * 60; // one day
        nTargetSpacing = 10 * 60;

        // Build the genesis block. Note that the output of the genesis coinbase cannot
        // be spent as it did not originally exist in the database.
        //
        // CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
        //   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
        //     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
        //     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
        //   vMerkleTree: 4a5e1e
        const char* pszTimestamp = "The Times web front page 22-Jul-2011 Europe hails 'historic' deal to save single currency";
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 50 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime    = 1311305081;
        genesis.nBits    = 0x1d00ffff;
        genesis.nNonce   = 3085127155;

        hashGenesisBlock = genesis.GetHash();
		assert(hashGenesisBlock == uint256("0x0000000062558fec003bcbf29e915cddfc34fa257dc87573f28e4520d1c7c11e"));
        assert(genesis.hashMerkleRoot == uint256("0xe61339a40aa4e90e983fe0d64cf09eed5fa1e6eac227b6761f06ac7af1929baf"));

        vSeeds.push_back(CDNSSeedData("devtome.com", "dvcstable01.devtome.com"));
        vSeeds.push_back(CDNSSeedData("dvcnode.org", "dvcstable01.dvcnode.org"));
        vSeeds.push_back(CDNSSeedData("dvcnode.org", "dvcstable02.dvcnode.org"));
        vSeeds.push_back(CDNSSeedData("dvcnode.org", "dvcstable03.dvcnode.org"));
        vSeeds.push_back(CDNSSeedData("dvcnode.org", "dvcstable04.dvcnode.org"));
        vSeeds.push_back(CDNSSeedData("dvcnode.org", "dvcstable05.dvcnode.org"));
		vSeeds.push_back(CDNSSeedData("dvcnode.org", "dvcstable06.dvcnode.org"));
		vSeeds.push_back(CDNSSeedData("dvcnode.org", "dvcstable07.dvcnode.org"));
		vSeeds.push_back(CDNSSeedData("dvcnode.com", "node01.dvcnode.com"));
		vSeeds.push_back(CDNSSeedData("dvcnode.com", "node02.dvcnode.com"));
		vSeeds.push_back(CDNSSeedData("dvcnode.com", "node03.dvcnode.com"));

        base58Prefixes[PUBKEY_ADDRESS] = list_of(0);
        base58Prefixes[SCRIPT_ADDRESS] = list_of(5);
        base58Prefixes[SECRET_KEY] =     list_of(128);
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x04)(0x88)(0xB2)(0x1E);
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x04)(0x88)(0xAD)(0xE4);

        convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));

        fRequireRPCPassword = true;
        fMiningRequiresPeers = true;
        fDefaultCheckMemPool = false;
        fAllowMinDifficultyBlocks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fSkipProofOfWorkCheck = false;
        fTestnetToBeDeprecatedFieldRPC = false;
    }

    const Checkpoints::CCheckpointData& Checkpoints() const 
    {
        return data;
    }
};
static CMainParams mainParams;

//
// Testnet (v3)
//

class CTestNetParams : public CMainParams {
public:
    CTestNetParams() {
        networkID = CBaseChainParams::TESTNET;
        strNetworkID = "test";
        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[0] = 'd';
        pchMessageStart[1] = 'e';
        pchMessageStart[2] = 'v';
        pchMessageStart[3] = '-';
        vAlertPubKey = ParseHex("04fc9702847840aaf195de8442ebecedf5b095cdbb9bc716bda9110971b28a49e0ead8564ff0db22209e0374782c093bb899692d524e9d6a6956e7c5ecbcd68284");
        nDefaultPort = 62333;
        nEnforceBlockUpgradeMajority = 51;
        nRejectBlockOutdatedMajority = 75;
        nToCheckBlockUpgradeMajority = 100;
		bnProofOfWorkLimit = ~uint256(0) >> 28;
        nMinerThreads = 0;
        nTargetTimespan = 24 * 60 * 60; // one day
        nTargetSpacing = 10 * 60;
		const char* pszTimestamp = "The Times web front page 22-Jul-2011 Europe hails 'historic' deal to save single currency";
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 50 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime    = 1311305081;
        genesis.nBits    = 0x1d00ffff;
        genesis.nNonce   = 3085127155;

        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0x4ecac46e98120fcf958f372a28ccfe505679a261e51e0c79e7ea24507c72bf04"));
		assert(genesis.hashMerkleRoot == uint256("0x92849cf14251e872336da6caf5be0cca44a71898d02393e1f698980a0cc4770b"));
        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("devtome.com", "dvcstable01.devtome.com"));
        vSeeds.push_back(CDNSSeedData("dvcnode.org", "dvcstable01.dvcnode.org"));
        vSeeds.push_back(CDNSSeedData("dvcnode.org", "dvcstable02.dvcnode.org"));
        vSeeds.push_back(CDNSSeedData("dvcnode.org", "dvcstable03.dvcnode.org"));
        vSeeds.push_back(CDNSSeedData("dvcnode.org", "dvcstable04.dvcnode.org"));
        vSeeds.push_back(CDNSSeedData("dvcnode.org", "dvcstable05.dvcnode.org"));
		vSeeds.push_back(CDNSSeedData("dvcnode.org", "dvcstable06.dvcnode.org"));
		vSeeds.push_back(CDNSSeedData("dvcnode.org", "dvcstable07.dvcnode.org"));
		vSeeds.push_back(CDNSSeedData("dvcnode.com", "node01.dvcnode.com"));
		vSeeds.push_back(CDNSSeedData("dvcnode.com", "node02.dvcnode.com"));
		vSeeds.push_back(CDNSSeedData("dvcnode.com", "node03.dvcnode.com"));

        base58Prefixes[PUBKEY_ADDRESS] = list_of(111);
        base58Prefixes[SCRIPT_ADDRESS] = list_of(196);
        base58Prefixes[SECRET_KEY]     = list_of(239);
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x04)(0x35)(0x87)(0xCF);
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x04)(0x35)(0x83)(0x94);

        convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));
		fSkipProofOfWorkCheck = false;
        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fDefaultCheckMemPool = false;
        fAllowMinDifficultyBlocks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = true;
    }
    const Checkpoints::CCheckpointData& Checkpoints() const 
    {
        return dataTestnet;
    }
};
static CTestNetParams testNetParams;

//
// Regression test
//
class CRegTestParams : public CTestNetParams {
public:
    CRegTestParams() {
        networkID = CBaseChainParams::REGTEST;
        strNetworkID = "regtest";
        pchMessageStart[0] = 'd';
        pchMessageStart[1] = 'e';
        pchMessageStart[2] = 'v';
        pchMessageStart[3] = 'r';
        nSubsidyHalvingInterval = 150;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 1;
        nTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        nTargetSpacing = 10 * 60;
        bnProofOfWorkLimit = ~uint256(0) >> 1;
        genesis.nTime = 1296688602;
        genesis.nBits = 0x207fffff;
        genesis.nNonce = 2;
        hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 52444;
       // assert(hashGenesisBlock == uint256("0x0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206"));

        vFixedSeeds.clear(); // Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();  // Regtest mode doesn't have any DNS seeds.

        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fDefaultCheckMemPool = true;
        fAllowMinDifficultyBlocks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;
    }
    const Checkpoints::CCheckpointData& Checkpoints() const 
    {
        return dataRegtest;
    }
};
static CRegTestParams regTestParams;

//
// Unit test
//
class CUnitTestParams : public CMainParams, public CModifiableParams {
public:
    CUnitTestParams() {
        networkID = CBaseChainParams::UNITTEST;
        strNetworkID = "unittest";
        nDefaultPort = 52445;
        vFixedSeeds.clear();
        vSeeds.clear();  // Regtest mode doesn't have any DNS seeds.

        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fDefaultCheckMemPool = true;
        fAllowMinDifficultyBlocks = false;
        fMineBlocksOnDemand = true;
    }

    const Checkpoints::CCheckpointData& Checkpoints() const 
    {
        // UnitTest share the same checkpoints as MAIN
        return data;
    }

    // Published setters to allow changing values in unit test cases
    virtual void setSubsidyHalvingInterval(int anSubsidyHalvingInterval)  { nSubsidyHalvingInterval=anSubsidyHalvingInterval; }
    virtual void setEnforceBlockUpgradeMajority(int anEnforceBlockUpgradeMajority)  { nEnforceBlockUpgradeMajority=anEnforceBlockUpgradeMajority; }
    virtual void setRejectBlockOutdatedMajority(int anRejectBlockOutdatedMajority)  { nRejectBlockOutdatedMajority=anRejectBlockOutdatedMajority; }
    virtual void setToCheckBlockUpgradeMajority(int anToCheckBlockUpgradeMajority)  { nToCheckBlockUpgradeMajority=anToCheckBlockUpgradeMajority; }
    virtual void setDefaultCheckMemPool(bool afDefaultCheckMemPool)  { fDefaultCheckMemPool=afDefaultCheckMemPool; }
    virtual void setAllowMinDifficultyBlocks(bool afAllowMinDifficultyBlocks) {  fAllowMinDifficultyBlocks=afAllowMinDifficultyBlocks; }
    virtual void setSkipProofOfWorkCheck(bool afSkipProofOfWorkCheck) { fSkipProofOfWorkCheck = afSkipProofOfWorkCheck; }
};
static CUnitTestParams unitTestParams;


static CChainParams *pCurrentParams = 0;

CModifiableParams *ModifiableParams()
{
   assert(pCurrentParams);
   assert(pCurrentParams==&unitTestParams);
   return (CModifiableParams*)&unitTestParams;
}

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams &Params(CBaseChainParams::Network network) {
    switch (network) {
        case CBaseChainParams::MAIN:
            return mainParams;
        case CBaseChainParams::TESTNET:
            return testNetParams;
        case CBaseChainParams::REGTEST:
            return regTestParams;
        case CBaseChainParams::UNITTEST:
            return unitTestParams;
        default:
            assert(false && "Unimplemented network");
            return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network) {
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectBaseParams(network);
    SelectParams(network);
    return true;
}
