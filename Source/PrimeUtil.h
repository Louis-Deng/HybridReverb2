/*
  ==============================================================================

    PrimeUtil.h
    Created: 29 Jul 2024 10:21:11am
    Author:  Louis Deng

  ==============================================================================
*/

#pragma once

#include <vector>
#include <cmath>

class primeArray
{
public:
    primeArray()
    {
        // only constructed once since static
        populateArr();
    }
    ~primeArray()
    {
    }
    
    /// returns the closest prime number to target, if returns -1 that means something is terribly wrong
    unsigned int closestPrime (float target, std::vector<unsigned int> * arrExclusionPtr) const
    {
        if (target<2.0f) target = 2.0f;
        if (target>maximumSample) target = maximumSample;
                
        /// estimated value in prime number catalogue
        int closestNum = -1;
        /// index of catalogue
        unsigned short int indC = 0;
        /// difference between *target* and *estimated prime numbers*
        short int closeNess = 4096;
        /// difference between *target* and *estimated neighbour prime numbers*
        short int neighbourDistance = 1;
        
        /**
         synopsis of this algorithm is to first estimate the closest prime number using a predetermined linear approximation of target float to index equation
         
         the algorithm (while loop below) recursively finds a new value of *estimate* while this is true:
                there exist *a prime number* -> call it the *next estimate*          (found by checking the *neighbouring primes* of the *estimate*)
                who can yield a smaller difference between
                    itself *next estimate* and _the target_
                than
                    *current estimate* and _the target_
         
         -----------------p-1---------p.----------p+1----------------...-------------------------------------------t--------------
         next iter
         ------------------------------p-1---------p.----------p+1---...-------------------------------------------t--------------
         ...
         until
         ...
         ----------------------------------------------------------------...------------------------p-1---------p.---t-------p+1--
         
         this means the algorithm (while loop) ends when _the target_ is closer or equaldistant (not further than) to the *estimate* than _it_ to *estimate's* neighbouring *primes*
         
         */
        
        // approximate indC first
        indC = floor( (target+240.36)/7.76 );
        
        // if target is too big, return biggest available prime number
        if (indC < 0) indC = 0;
        if (indC > arrPrime.size()-1) indC = arrPrime.size()-1;
        
        // judge closeness of the target:X and estimate:C
        while(abs(closeNess)>neighbourDistance)
        {
            closestNum = arrPrime[indC];
            closeNess = round(closestNum-target);
            
            if (indC == arrPrime.size()-1 )
            {
                neighbourDistance = round(target-arrPrime[arrPrime.size()-2]);
            }
            else if (indC == 0)
            {
                neighbourDistance = round(arrPrime[1]-target);
            }
            else
            {
                neighbourDistance = round(std::min( target-arrPrime[indC-1] , arrPrime[indC+1]-target ));
            }
            
            
            if (closeNess>0)
            {
                indC-=1;
            }
            else if (closeNess<0)
            {
                indC+=1;
            }
        }
        
        // if exclusion vector is not provided
        if (arrExclusionPtr == nullptr) return closestNum;
    
        /**
         the below code is a draft for output value exclusion. in case i have to exclude some prime numbers as lengths for the AP filters
         so to do that, probably have to make this entire class non static in initialization. because
         it's easier to take out a member of the vector and run the algorithm above until it finds the closest prime
         or think of another way to spit out closestNum without affecting the array (more sane) -> make a copy of the static thingy?
         
         */
        
        // find if the number is in the exclusion criteria
        bool numExcluded = false;
        for (int indE=0;indE<arrExclusionPtr->size();indE++)
        {
            
            // check if included in blacklist
            if (closestNum == (*arrExclusionPtr)[indE])
            {
                numExcluded = true;
                break;
            }
            // give the next prime
            if (numExcluded == true)
            {
                indC++;
                if (indC<=arrExclusionPtr->size())
                {
                    closestNum = arrPrime[indC];
                }else{
                    closestNum = -1;
                }
                
            }
        }
        
        
        return closestNum;
    }
    
private:
    std::vector<unsigned int> arrPrime;
    
    float maximumSample = 90000.0f;
    
    void populateArr()
    {
        arrPrime.clear();
        arrPrime.push_back(2);
        for (int num=3;num<90000;num+=2)
        {
            bool checkpoint = false;
            for (int i=0;i<arrPrime.size();i++)
            {
                if (num%arrPrime[i]==0)
                {
                    checkpoint = true;
                    break;
                }
                if (arrPrime[i]>sqrt(num))
                {
                    break;
                }
            }
            if (!checkpoint) arrPrime.push_back(num);
        }
    }
    
};
