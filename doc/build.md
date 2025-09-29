
## ⚙️ Build & Usage

```bash
# Clone repo
git clone https://github.com/yourname/micro_core.git
cd micro_core

# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build

# Run tests
ctest --test-dir build