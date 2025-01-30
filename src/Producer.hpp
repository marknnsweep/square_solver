#ifndef PRODUCER_HPP
#define PRODUCER_HPP

#include <atomic>
#include <string>
#include <tuple>

#include "PipelineQueue.hpp"
#include "TupleProducer.hpp"

class Producer {
public:
  using Data = std::vector<std::string>;
  using DataProducer = TupleProducer<Data, 3>;
  using StringCoefficients = typename DataProducer::TupleType;

private:
  const Data &inputData;
  PipelineQueue<StringCoefficients> &inputPipeline;

public:
  Producer(Data &inputData, PipelineQueue<StringCoefficients> &inputPipeline)
      : inputData(inputData), inputPipeline(inputPipeline) {}

  void run() {
    DataProducer dp(inputData);
    for (auto it = dp.begin(); it != dp.end(); ++it) {
      inputPipeline.enqueue(*it);
    }
    inputPipeline.done();
  }
};

#endif // PRODUCER_HPP