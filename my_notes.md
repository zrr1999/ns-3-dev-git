## 安装
```sh
paru -Sy boost
paru -Sy icu74 # 在 archlinux 的较新版本中, 官方仓库的 icu 版本是 75, 但是 ns3 需要 74

git clone https://github.com/nsnam/ns-3-dev-git.git
cd ns-3-dev-git/

./ns3 configure --enable-examples --enable-tests -- -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
./ns3 build
```

## 测试
```sh
./test.py
```
没有报错证明安装成功且在当前环境不存在影响运行的 bug。
