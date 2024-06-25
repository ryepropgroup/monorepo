mkdir -p generated
./vendor/protoc/protoc-27.1-win64/bin/protoc \
       --plugin=protoc-gen-nanopb=/vendor/nanopb/generator/protoc-gen-nanopb.bat \
       --nanopb_out=generated \
       -I"../protobuf/" \
       main.proto
