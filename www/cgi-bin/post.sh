#!/bin/sh
if [[ $REQUEST_METHOD != "POST" ]]; then
    echo "Status: 405";
    echo ;
    exit;
fi
if [ -z "$PATH_INFO" ] || [ -z "$CONTENT_LENGTH" ]; then
    echo "Status: 409";
    echo ;
    exit ;
fi
mkdir -p $(dirname "$PATH_INFO");
if [[ $? -ne 0 ]]; then
    echo "Status: 500";
    echo ;
    exit ;
fi
touch $PATH_INFO;
if [[ $? -ne 0 ]]; then
    echo "Status: 500";
    echo ;
    exit ;
fi
echo "$CONTENT_LENGTH";
bytes_read=0
while [ "$bytes_read" -lt "$CONTENT_LENGTH" ]; do
    chunk_size=$((CONTENT_LENGTH - bytes_read))
    IFS= read -r -n "$chunk_size" chunk
    bytes_read=$((bytes_read + ${#chunk}))
    stdin="$stdin$chunk"
done
echo "read";
if [[ -z "$stdin" ]]; then
    echo "Status: 500";
    echo ;
    exit ;
fi
echo "$stdin" > "$PATH_INFO";
echo "wrote";
if [[ $? -ne 0 ]]; then
    echo "Status: 500";
    echo ;
    exit ;
fi
echo "Status: 201";
echo ;
exit 0;