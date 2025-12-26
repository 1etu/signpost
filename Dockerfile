FROM alpine:latest AS builder

RUN apk add --no-cache build-base

WORKDIR /src
COPY . .
RUN make

FROM alpine:latest

RUN adduser -D -H signpost

COPY --from=builder /src/signpost /usr/local/bin/
COPY config.json /etc/signpost/config.json

USER signpost
EXPOSE 25565

CMD ["signpost", "/etc/signpost/config.json"]
