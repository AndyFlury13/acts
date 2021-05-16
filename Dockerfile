FROM ghcr.io/acts-project/ubuntu2004:v9

# system dependencies
RUN apt-get update && \
    apt-get install -y clang && \
    apt-get clean -y
