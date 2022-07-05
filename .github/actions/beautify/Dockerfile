FROM alpine:latest

ADD stylecheck /usr/local/bin/stylecheck
RUN chmod +x /usr/local/bin/stylecheck
RUN apk add --no-cache git indent
