CXX = g++

CXXFLAGS = -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations \
           -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal \
           -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd \
           -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel \
           -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default \
           -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers \
           -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow \
           -flto-odr-type-merging -fno-omit-frame-pointer -Wstack-usage=8192 -pie -fPIE -Werror=vla                                                     \
           -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

LDFLAGS = -L/usr/local/lib -lsfml-graphics -lsfml-window -lsfml-system

SOURCES = main.cpp akinatorGUI.cpp BTree/tree.cpp GraphDump/graph_dump.cpp ../lib/logger/logger.cpp ../lib/my_stack/src/stack.cpp ../lib/my_stack/src/debug.cpp
INCLUDES = -I./common -I./logger -I./BTree -I./../lib/logger -I./../lib/my_stack/include

OBJECTS = $(SOURCES:%.cpp=build/%.o)

.PHONY: all
all: do.exe
	@echo -e "\033[33mCompilation complete.\033[0m"

do.exe: $(OBJECTS)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) $^ $(LDFLAGS) -o do

build/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean
clean:
	rm -rf do.exe build


