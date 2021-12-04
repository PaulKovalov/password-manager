FROM alpine:3.14

RUN apk update && apk add --no-cache \
    gcc \
    # musl-dev is the same as libc-dev. Dependency required for gcc.
    musl-dev \
    python3 \
    python3-dev \
    py-pip

# Create test user
RUN addgroup -S appgroup && adduser -S Vasiliy -G appgroup

WORKDIR home/Vasiliy
COPY requirements.txt ./
RUN pip install -r requirements.txt --ignore-installed

COPY *.py ./
COPY testing/*.py ./
COPY testing/run_tests.sh ./
RUN chmod +x run_tests.sh
# Run next command as Vasiliy
USER Vasiliy
CMD ["/bin/sh", "run_tests.sh"]