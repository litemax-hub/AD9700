#!/bin/bash

CORE_NUM=${1:-12}
LOG_FILE="build_output.log"

make realclean

echo "Starting build with $CORE_NUM cores..."

# 1. 執行 make，將 stderr (2) 導向 stdout (1)，並同時輸出到螢幕與檔案
# 2. 使用 PIPESTATUS 陣列來獲取管道中第一個命令 (make) 的回傳值，而非 tee 的回傳值
time make -j"$CORE_NUM" 2>&1 | tee "$LOG_FILE"
MAKE_EXIT_CODE=${PIPESTATUS[0]}

# 檢查 make 是否成功
if [ $MAKE_EXIT_CODE -eq 0 ]; then
    echo "----------------------------------------"
    echo "編譯成功！(Exit Code: 0)"

    # 邏輯分析：
    # 1. grep "警告：" 找出所有警告行
    # 2. grep -v "#warning" 排除掉開發者主動留下的 #warning 提示
    # 3. wc -l 計算行數
    
    REAL_WARNINGS=$(grep "警告：" "$LOG_FILE" | grep -v "#warning")
    WARNING_COUNT=$(echo "$REAL_WARNINGS" | grep -c "警告：")

    if [ "$WARNING_COUNT" -gt 0 ]; then
        echo -e "\033[33m[注意] 檢測到 $WARNING_COUNT 個標準編譯器警告 (已排除 #warning)：\033[0m"
        echo "----------------------------------------"
        echo "$REAL_WARNINGS"
        echo "----------------------------------------"
    else
        echo -e "\033[32m[完美] 沒有發現標準編譯器警告 (Clean Build)。\033[0m"
    fi

else
    echo -e "\033[31m編譯失敗！\033[0m"
fi

date