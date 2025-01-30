#ifndef PRODUCER_HPP
#define PRODUCER_HPP

#include <atomic>
#include <string>
#include <tuple>

#include "PipelineQueue.hpp"
#include "TupleProducer.hpp"

class Producer {
public:
  using StringCoefficients =
      std::tuple<std::optional<std::string>, std::optional<std::string>,
                 std::optional<std::string>>;
  using Data = std::vector<std::string>;

private:
  const Data &inputData;
  PipelineQueue<StringCoefficients> &inputPipeline;

public:
  Producer(Data &inputData, PipelineQueue<StringCoefficients> &inputPipeline)
      : inputData(inputData), inputPipeline(inputPipeline) {}

  void run() {
    TupleProducer<Data, 3> tp(inputData);
    for (auto it = tp.begin(); it != tp.end(); ++it) {
      inputPipeline.enqueue(*it);
    }
    inputPipeline.done();
  }
};

#endif // PRODUCER_HPP