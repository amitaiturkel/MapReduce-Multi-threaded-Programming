
# MapReduce Multi-threaded Framework

## Overview

Welcome to the MapReduce Multi-threaded Framework! 🚀

This project is all about making big data processing faster and more efficient. By leveraging the power of multi-threading, our framework breaks down large datasets into manageable chunks, processes them in parallel, and then combines the results. Whether you're handling text analysis, large-scale computations, or any task that benefits from parallel processing, this framework is designed to help you get the job done quickly and efficiently.

### Authors

- **Amitai Turkel**

## Build Instructions

### Static Library

At the heart of this project is the MapReduce framework itself. To make it easy to integrate into your own projects, we've provided a `Makefile` in the root directory that compiles everything into a static library, `libMapReduceFramework.a`.

Here’s how you can build the library:

```bash
make
```

This command compiles all the core components of the framework and packages them into a static library that you can link against in your projects. It’s like building a powerful tool that you can reuse anytime you need it!

When you're done or if you want to start fresh, clean up the build artifacts with:

```bash
make clean
```

### Client Example

To see the framework in action, we've included a sample client in the `SampleClient` folder. This client demonstrates how you can use the framework to solve real-world problems.

To try it out:

1. First, navigate to the `SampleClient` folder:

   ```bash
   cd SampleClient
   ```

2. Then, build the client:

   ```bash
   make
   ```

3. Run the client to see the magic of multi-threaded MapReduce:

   ```bash
   make run
   ```

4. If you need to clean up:

   ```bash
   make clean
   ```

This example is a great starting point if you're new to the framework. It shows how to set up a MapReduce job, feed it data, and handle the results—all in a multi-threaded environment.

## License

This project is open-source and available under the [MIT License](LICENSE).

---

Feel free to explore, experiment, and expand on this framework. We're excited to see what you’ll build with it! 😊
