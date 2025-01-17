/*
 The MIT License (MIT)

 Copyright (c) [2016] [BTC.COM]

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
#ifndef GBT_MAKER_H_
#define GBT_MAKER_H_

#include "Common.h"
#include "Kafka.h"

#include "zmq.hpp"

/////////////////////////////////// GbtMaker ///////////////////////////////////
class GbtMaker {
  atomic<bool> running_;
  mutex lock_;

  std::unique_ptr<zmq::context_t> zmqContext_;
  string zmqBitcoindAddr_;
  uint32_t zmqTimeout_;

  string bitcoindRpcAddr_;
  string bitcoindRpcUserpass_;
  atomic<uint32_t> lastGbtMakeTime_;
#if defined(CHAIN_TYPE_BCH) || defined(CHAIN_TYPE_BSV) || defined(CHAIN_TYPE_FCH)
  atomic<uint32_t> lastGbtLightMakeTime_;
#endif
  uint32_t kRpcCallInterval_;

  string kafkaBrokers_;
  string kafkaRawGbtTopic_;
  KafkaProducer kafkaProducer_;
  bool isCheckZmq_;

  bool bitcoindRpcGBT(string &resp);
  string makeRawGbtMsg();
  void submitRawGbtMsg(bool checkTime);

#if defined(CHAIN_TYPE_BCH) || defined(CHAIN_TYPE_BSV) || defined(CHAIN_TYPE_FCH)
  bool bitcoindRpcGBTLight(string &resp);
  string makeRawGbtLightMsg();
  void submitRawGbtLightMsg(bool checkTime);
#endif

  void threadListenBitcoind();

  void kafkaProduceMsg(const void *payload, size_t len);

public:
  GbtMaker(
      const string &zmqBitcoindAddr,
      uint32_t zmqTimeout,
      const string &bitcoindRpcAddr,
      const string &bitcoindRpcUserpass,
      const string &kafkaBrokers,
      const string &kafkaRawGbtTopic,
      uint32_t kRpcCallInterval,
      bool isCheckZmq);
  ~GbtMaker();

  bool init();
  void stop();
#if defined(CHAIN_TYPE_BCH) || defined(CHAIN_TYPE_BSV) || defined(CHAIN_TYPE_FCH)
  void runLightGbt();
#endif
  void run();
};

//////////////////////////////// NMCAuxBlockMaker //////////////////////////////
//
// rpc call: ./namecoin-cli createauxblock N59bssPo1MbK3khwPELTEomyzYbHLb59uY
//
class NMCAuxBlockMaker {
  atomic<bool> running_;
  mutex lock_;

  std::unique_ptr<zmq::context_t> zmqContext_;
  string zmqNamecoindAddr_;
  uint32_t zmqTimeout_;

  string rpcAddr_;
  string rpcUserpass_;
  atomic<uint32_t> lastCallTime_;
  uint32_t kRpcCallInterval_;
  string fileLastRpcCallTime_;

  string kafkaBrokers_;
  string kafkaAuxPowGwTopic_;
  KafkaProducer kafkaProducer_;
  bool isCheckZmq_;
  string coinbaseAddress_; // nmc coinbase payout address
  bool useCreateAuxBlockInterface_;

  bool callRpcCreateAuxBlock(string &resp);
  string makeAuxBlockMsg();

  void submitAuxblockMsg(bool checkTime);
  void threadListenNamecoind();

  void kafkaProduceMsg(const void *payload, size_t len);

public:
  NMCAuxBlockMaker(
      const string &zmqNamecoindAddr,
      uint32_t zmqTimeout,
      const string &rpcAddr,
      const string &rpcUserpass,
      const string &kafkaBrokers,
      const string &kafkaAuxPowGwTopic,
      uint32_t kRpcCallInterval,
      const string &fileLastRpcCallTime,
      bool isCheckZmq,
      const string &coinbaseAddress);
  ~NMCAuxBlockMaker();

  bool init();
  void stop();
  void run();
};

#endif
