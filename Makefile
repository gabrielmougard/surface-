# Copyright 2022 gab
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

ASSIMP_SOURCES=third_party/assimp

all:
	mkdir -p build
	cmake -S . -B build -G Ninja
	cmake --build build

init:
	git submodule update --init --recursive
	git -C $(ASSIMP_SOURCES) checkout v5.0.1
	cmake -S $(ASSIMP_SOURCES) -B $(ASSIMP_SOURCES)
	make -C $(ASSIMP_SOURCES) -j4

clean:
	rm -rf build
	git clean -e third_party/* build/* CMakeFiles CMakeCache.txt -f