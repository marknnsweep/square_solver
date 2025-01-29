#ifndef PRODUCER_HPP
#define PRODUCER_HPP

#include <atomic>
#include <string>
#include <tuple>

#include "SortOfLockFreeQueue.hpp"
#include "TupleProducer.hpp"

class Producer {
public:
  using StringCoefficients =
      std::tuple<std::optional<std::string>, std::optional<std::string>,
                 std::optional<std::string>>;
  using Data = std::vector<std::string>;

private:
  std::atomic<bool> &done_flag;

  Data &inputData;
  SortOfLockFreeQueue<StringCoefficients> &inputQueue;

public:
  Producer(Data &inputData, SortOfLockFreeQueue<StringCoefficients> &inputQueue,
           std::atomic<bool> &df)
      : inputData(inputData), inputQueue(inputQueue), done_flag(df) {}

  void run() {
    TupleProducer<Data, 3> tp(inputData);
    for (auto it = tp.begin(); it != tp.end(); ++it) {
      inputQueue.enqueue(*it);
    }
    done_flag.store(true, std::memory_order_release);
  }
};

#endif // PRODUCER_HPP