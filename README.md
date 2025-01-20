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
- YARP 3.7
- iCub Main 1.25.0 (with simulator)

For detailed installation instructions, see [INSTALL.md](INSTALL.md).

## Project Structure

The project is organized into the following modules:

```
src/
├── types/
│   └── GazeTypes.h           # Common types and constants
├── control/
│   ├── RobotHeadControl.h    # Robot head movement control
│   ├── RobotHeadControl.cpp
│   ├── SearchBehavior.h      # Natural search pattern behavior
│   └── SearchBehavior.cpp
├── vision/
│   ├── SphereDetector.h      # Target detection algorithms
│   └── SphereDetector.cpp
├── GazeController.h          # Main controller
└── GazeController.cpp
```

### Modules

1. **Types Module** (`types/`)
   - Defines common types and constants used across the system
   - Includes control parameters and state definitions

2. **Control Module** (`control/`)
   - `RobotHeadControl`: Manages robot head and eye movements
   - `SearchBehavior`: Implements natural search patterns

3. **Vision Module** (`vision/`)
   - `SphereDetector`: Handles target detection and tracking

4. **Main Controller**
   - Coordinates the interaction between modules
   - Implements state machine for search and tracking behavior

## Features

- Human-like search behavior with natural head movements
- Smooth head-eye coordination during tracking
- Automatic switching between search and tracking modes
- Maintains level head posture
- Robust target detection

## Dependencies

- YARP
- C++17 or later
- CMake 3.5 or later

## Building

```bash
mkdir build && cd build
cmake ..
make
```

## Running

1. Start the YARP server:
```bash
yarpserver
```

2. Start the iCub simulator:
```bash
iCub_SIM
```

3. Run the eye tracking program:
```bash
./bin/icub_eye_tracking
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

## Recent Changes

- Refactored code into modular components for better maintainability
- Added roll control to keep head level during movements
- Improved search pattern with wider range of motion
- Enhanced error handling and debugging output
- Added proper cleanup in destructors

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
