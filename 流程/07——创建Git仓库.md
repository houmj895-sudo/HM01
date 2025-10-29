### 🪄 第 1 步：重命名主分支为 `main`

```bash
git branch -m main
```

这会把你当前的分支名从 `master` 改为 `main`。
执行后你可以用 `git status` 验证一下，会看到：

```
On branch main
```

---

### 🪄 第 2 步：提交项目初始版本

```bash
git init #创建一个空的 Git 仓库或重新初始化一个已存在的仓库
git add .
git commit -m "Week1: initialize project structure"
```

这两条命令的作用：

* `git add .`：把项目当前文件变化（目录结构、README、脚本等）加入到 Git 的暂存区；
* `git commit -m "..."`：把这些变化打包成一个版本（也就是第一个快照）。

---

💡 完成后，你的项目就正式有了：

* **main 分支**（标准命名）
* **第一个提交记录**（代表 Week1 的环境初始化）

---

执行完告诉我输出，我帮你确认是否提交成功。

git log产看所有提交
git reset --hard 哈希值可以强制回退到指定版本
