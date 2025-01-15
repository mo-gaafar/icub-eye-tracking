# iCub Development Environment (Windows)

## Prerequisites

* [Docker Desktop for Windows](https://www.docker.com/products/docker-desktop)

## Usage

1. Clone the repository:
    ```bash
    git clone https://github.com/zeyadmohaymen/icub-docker-environment.git
    ```

2. Navigate to the repository and run the development environment:
    ```bash
    ./run
    ```

## Notes

- Source code goes in `src/` directory
- Container mounts `./src` to `/workspace`
- Uses Windows-specific Docker configuration
