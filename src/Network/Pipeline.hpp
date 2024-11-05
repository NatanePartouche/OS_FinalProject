#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <vector>
#include <functional>

// The Pipeline class allows for sequential execution of a series of steps,
// each represented by a function with no arguments and no return value.
class Pipeline {
public:
    // Adds a new step to the pipeline. Each step is a function
    // that will be executed sequentially when `execute()` is called.
    void addStep(std::function<void()> step);

    // Executes all the steps in the pipeline in the order they were added.
    // This function is marked as `const` because it does not modify the pipeline itself.
    void execute() const;

private:
    // Stores each step to be executed, as a vector of functions.
    std::vector<std::function<void()>> steps;
};

#endif // PIPELINE_HPP