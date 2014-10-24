// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "pow.h"

#include "chainparams.h"
#include "core.h"
#include "main.h"
#include "timedata.h"
#include "uint256.h"
#include "util.h"
// Start accepting AUX POW at this block
// 
// Even if we do not accept AUX POW ourselves, we can always be the parent chain.
 
inline int GetAuxPowStartBlock()
{
    if (TestNet())
        return 0; // Always on testnet
    else
        return 25000; // Never on prodnet
}
inline int GetOurChainID()
{
    return 0x0004;
}
unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock)
{
	if(pindexLast->nHeight >= 150000)
	{
		GetNextWorkRequired_Original(pindexLast, pblock);
	}
	else
	{
		GetNextWorkRequired_Old(pindexLast, pblock);
	}
}
unsigned int GetNextWorkRequired_Old(const CBlockIndex* pindexLast, const CBlockHeader *pblock)
{
    unsigned int nProofOfWorkLimit = Params().ProofOfWorkLimit().GetCompact();

    const int nSmoothBlock = 10700;
    const int64_t nTargetSpacing = Params().TargetSpacing();
    const int64_t nTargetTimespan = Params().TargetTimespan(); // one day

    if (pindexLast->nHeight < nSmoothBlock)
        nTargetTimespan *= 14; // two weeks

    const int64_t nInterval = nTargetTimespan / nTargetSpacing;

    // Genesis block
    if (pindexLast == NULL)
        return nProofOfWorkLimit;

    const int nMedianBlock = 10800;
    int64_t nIntervalMinusOne = nInterval-1;

    if (pindexLast->nHeight < 10)
        return pindexLast->nBits;


    // Change at each block after nSmoothBlock
    if (pindexLast->nHeight < nSmoothBlock)
        if ((pindexLast->nHeight+1) % nInterval != 0)
		{
			// Special difficulty rule for testnet:
			if (TestNet() == true)
			{
				// If the new block's timestamp is more than 2* 10 minutes
				// then allow mining of a min-difficulty block.
				if (pblock->nTime > pindexLast->nTime + nTargetSpacing*2)
					return nProofOfWorkLimit;
				else
				{
					// Return the last non-special-min-difficulty-rules-block
					const CBlockIndex* pindex = pindexLast;
					while (pindex->pprev && pindex->nHeight % nInterval != 0 && pindex->nBits == nProofOfWorkLimit)
						pindex = pindex->pprev;
					return pindex->nBits;
				}
			}
            return pindexLast->nBits;
		}

    // Go back by what we want to be one day worth of blocks
    const CBlockIndex* pindexFirst = pindexLast;
    vector<int64_t> blockTimes;
    CBigNum averageBits;
    averageBits.SetCompact(0);

    for (int i = 0; pindexFirst && i < nIntervalMinusOne; i++)
    {
        averageBits += CBigNum().SetCompact(pindexFirst->nBits);
        blockTimes.push_back(pindexFirst->GetBlockTime());
        pindexFirst = pindexFirst->pprev;
    }

    assert(pindexFirst);
    int blockTimeEndIndex = blockTimes.size() - 6;
    sort(blockTimes.begin(), blockTimes.end());
    averageBits /= nIntervalMinusOne;

    // Limit adjustment step
    int64_t nActualTimespan = pindexLast->GetBlockTime() - pindexFirst->GetBlockTime();
    int64_t nMedianTimespan = blockTimes[blockTimeEndIndex] - blockTimes[6];
    nMedianTimespan *= nIntervalMinusOne / (int64_t)(blockTimeEndIndex - 6);

    // Change nActualTimespan after nMedianBlock
    if (pindexLast->nHeight > nMedianBlock)
    {
        nActualTimespan = nMedianTimespan;
    }

  //  printf(" nActualTimespan = %"PRI64d" before bounds\n", nActualTimespan);

    if (nActualTimespan < nTargetTimespan/4)
        nActualTimespan = nTargetTimespan/4;

    if (nActualTimespan > nTargetTimespan*4)
        nActualTimespan = nTargetTimespan*4;

    // Retarget
    CBigNum bnNew;
    bnNew.SetCompact(pindexLast->nBits);

    // Change bnNew after nMedianBlock
    if (pindexLast->nHeight > nMedianBlock)
        bnNew = averageBits;

    bnNew *= nActualTimespan;
    bnNew /= nTargetTimespan;

    if (bnNew > Params().ProofOfWorkLimit())
        bnNew = Params().ProofOfWorkLimit();

    /// debug print
    /*printf("GetNextWorkRequired RETARGET\n");
    printf("nTargetTimespan = %"PRI64d" nActualTimespan = %"PRI64d"\n", nTargetTimespan, nActualTimespan);
    printf("Before: %08x %s\n", pindexLast->nBits, CBigNum().SetCompact(pindexLast->nBits).getuint256().ToString().c_str());
    printf("After: %08x %s\n", bnNew.GetCompact(), bnNew.getuint256().ToString().c_str());*/

    return bnNew.GetCompact();
}
unsigned int GetNextWorkRequired_Original(const CBlockIndex* pindexLast, const CBlockHeader *pblock)
{
    unsigned int nProofOfWorkLimit = Params().ProofOfWorkLimit().GetCompact();

    // Genesis block
    if (pindexLast == NULL)
        return nProofOfWorkLimit;

    // Only change once per interval
    if ((pindexLast->nHeight+1) % nInterval != 0)
    {
        if (TestNet())
        {
            // Special difficulty rule for testnet:
            // If the new block's timestamp is more than 2* 10 minutes
            // then allow mining of a min-difficulty block.
            if (pblock->nTime > pindexLast->nTime + Params().TargetSpacing()*2)
                return nProofOfWorkLimit;
            else
            {
                // Return the last non-special-min-difficulty-rules-block
                const CBlockIndex* pindex = pindexLast;
                while (pindex->pprev && pindex->nHeight % nInterval != 0 && pindex->nBits == nProofOfWorkLimit)
                    pindex = pindex->pprev;
                return pindex->nBits;
            }
        }
        return pindexLast->nBits;
    }

    // Go back by what we want to be 14 days worth of blocks
    const CBlockIndex* pindexFirst = pindexLast;
    for (int i = 0; pindexFirst && i < nInterval-1; i++)
        pindexFirst = pindexFirst->pprev;
    assert(pindexFirst);

    // Limit adjustment step
    int64_t nActualTimespan = pindexLast->GetBlockTime() - pindexFirst->GetBlockTime();
    LogPrintf("  nActualTimespan = %d  before bounds\n", nActualTimespan);
    if (nActualTimespan < Params().TargetTimespan()/4)
        nActualTimespan = Params().TargetTimespan()/4;
    if (nActualTimespan > Params().TargetTimespan()*4)
        nActualTimespan = Params().TargetTimespan()*4;

    // Retarget
    CBigNum bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    bnNew *= nActualTimespan;
    bnNew /= Params().TargetTimespan();

    if (bnNew > Params().ProofOfWorkLimit())
        bnNew = Params().ProofOfWorkLimit();

    /// debug print
    LogPrintf("GetNextWorkRequired RETARGET\n");
    LogPrintf("nTargetTimespan = %d    nActualTimespan = %d\n", Params().TargetTimespan(), nActualTimespan);
    LogPrintf("Before: %08x  %s\n", pindexLast->nBits, CBigNum().SetCompact(pindexLast->nBits).getuint256().ToString());
    LogPrintf("After:  %08x  %s\n", bnNew.GetCompact(), bnNew.getuint256().ToString());

    return bnNew.GetCompact();
}



void CBlockHeader::SetAuxPow(CAuxPow* pow) {
	if (pow != NULL)
		nVersion |= BLOCK_VERSION_AUXPOW;
	else
		nVersion &= ~BLOCK_VERSION_AUXPOW;
	auxpow.reset(pow);
}

bool CBlockHeader::CheckProofOfWork(int nHeight) const {
	if (nHeight >= GetAuxPowStartBlock()) {
		// Prevent same work from being submitted twice:
		// - this block must have our chain ID
		// - parent block must not have the same chain ID (see CAuxPow::Check)
		// - index of this chain in chain merkle tree must be pre-determined (see CAuxPow::Check)
		if (!TestNet() && nHeight != INT_MAX && GetChainID() != GetOurChainID())
			return error(
					"CheckProofOfWork() : block does not have our chain ID");

		if (auxpow.get() != NULL) {
			if (!auxpow->Check(GetHash(), GetChainID()))
				return error("CheckProofOfWork() : AUX POW is not valid");
			// Check proof of work matches claimed amount
			if (!::CheckProofOfWork(auxpow->GetParentBlockHash(), nBits))
				return error("CheckProofOfWork() : AUX proof of work failed");
		} else {
			// Check proof of work matches claimed amount
			if (!::CheckProofOfWork(GetPoWHash(), nBits))
				return error("CheckProofOfWork() : proof of work failed");
		}
	} else {
		if (auxpow.get() != NULL) {
			return error(
					"CheckProofOfWork() : AUX POW is not allowed at this block");
		}

		// Check if proof of work marches claimed amount
		if (!::CheckProofOfWork(GetPoWHash(), nBits))
			return error("CheckProofOfWork() : proof of work failed");
	}
	return true;
}
bool CheckProofOfWork(uint256 hash, unsigned int nBits)
{
    bool fNegative;
    bool fOverflow;
    uint256 bnTarget;

    if (Params().SkipProofOfWorkCheck())
       return true;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > Params().ProofOfWorkLimit())
        return error("CheckProofOfWork() : nBits below minimum work");

    // Check proof of work matches claimed amount
    if (hash > bnTarget)
        return error("CheckProofOfWork() : hash doesn't match nBits");

    return true;
}

void UpdateTime(CBlockHeader* pblock, const CBlockIndex* pindexPrev)
{
    pblock->nTime = std::max(pindexPrev->GetMedianTimePast()+1, GetAdjustedTime());

    // Updating time can change work required on testnet:
    if (Params().AllowMinDifficultyBlocks())
        pblock->nBits = GetNextWorkRequired(pindexPrev, pblock);
}

uint256 GetProofIncrement(unsigned int nBits)
{
    uint256 bnTarget;
    bool fNegative;
    bool fOverflow;
    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);
    if (fNegative || fOverflow || bnTarget == 0)
        return 0;
    // We need to compute 2**256 / (bnTarget+1), but we can't represent 2**256
    // as it's too large for a uint256. However, as 2**256 is at least as large
    // as bnTarget+1, it is equal to ((2**256 - bnTarget - 1) / (bnTarget+1)) + 1,
    // or ~bnTarget / (nTarget+1) + 1.
    return (~bnTarget / (bnTarget + 1)) + 1;
}
