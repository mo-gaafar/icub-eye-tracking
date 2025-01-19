# iCub Eye Tracking System

This repository implements a gaze control system for the iCub robot simulator, focusing on fast and accurate gaze shifts towards visual targets.

## Overview

The system implements a sophisticated gaze control mechanism with the following features:
- Real-time detection and tracking of a red sphere in the environment
- Two-phase gaze control strategy:
  1. Fast initial eye movement towards the target
  2. Coordinated head-eye movement for precise tracking
- Integration with iCub simulator and YARP middleware

## Prerequisites

- Ubuntu 22.04 (Jammy)
- YARP 3.10.1
- iCub Main (with simulator)
- OpenCV and other dependencies

For detailed installation instructions, see [INSTALL.md](INSTALL.md).

## Project Structure

```
src/
  ├── controllers/     # Eye and head movement controllers
  │   ├── GazeController.cpp    # Main gaze control implementation
  │   └── HeadController.cpp    # Head movement coordination
  ├── vision/         # Image processing and sphere detection
  │   ├── SphereDetector.cpp    # Red sphere detection algorithms
  │   └── ImageProcessor.cpp    # Camera image processing
  ├── utils/          # Utility functions and common code
  │   ├── YarpHelper.cpp        # YARP interface utilities
  │   └── Math.cpp              # Mathematical utilities
  ├── test/          # Unit tests
  │   ├── test_sphere_detector.cpp
  │   └── test_gaze_controller.cpp
  └── main.cpp        # Main application entry point
```

## Building the Project

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Running the Application

1. Start the YARP server:
```bash
yarpserver
```

2. In a new terminal, launch the iCub simulator:
```bash
iCub_SIM
```

3. In another terminal, run the eye tracking application:
```bash
cd build
./icub_eye_tracking
```

## Development Guidelines

1. Code Quality
   - Write unit tests for all new backend code
   - Follow C++ best practices and SOLID principles
   - Use consistent code formatting

2. Documentation
   - Update documentation for any new features or changes
   - Keep the changelog current
   - Document complex algorithms and design decisions

3. Testing
   - Run unit tests before committing: `make test`
   - Test with different sphere positions and movements
   - Verify both phases of gaze control

## Changelog

### [0.2.0] - 2025-01-19
- Migrated from Docker to native Ubuntu environment
- Added detailed installation guide
- Improved project documentation
- Added unit test framework
- Updated build system for better dependency management

### [0.1.0] - 2025-01-15
- Initial project setup
- Basic project structure
- Documentation for setup and requirements

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributing

1. Fork the repository
2. Create your feature branch
3. Write and test your changes
4. Update documentation and changelog
5. Submit a pull request

## Acknowledgments

- iCub Robotics team for the simulator and support
- YARP development team
- Contributors and maintainers
