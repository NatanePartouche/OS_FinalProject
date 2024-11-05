#include "Pipeline.hpp"

// Adds a new step to the pipeline by moving the given function into the steps vector.
// Using std::move optimizes by avoiding an unnecessary copy of the function object.
void Pipeline::addStep(std::function<void()> step){
    steps.push_back(std::move(step));
}

// Executes each step in the pipeline in the order they were added.
// The `const` qualifier ensures that execute does not modify the pipeline.
void Pipeline::execute() const {
    for (const auto &step : steps) {
        step(); // Call each step (function) sequentially
    }
}