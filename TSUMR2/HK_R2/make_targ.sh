#!/bin/bash

CORE_NUM=${1:-12}
LOG_FILE="build_output.log"

make realclean

echo "Starting build with $CORE_NUM cores..."

# 1. 執行 make，將 stderr (2) 導向 stdout (1)，並同時輸出到螢幕與檔案
# 2. 使用 PIPESTATUS 陣列來獲取管道中第一個命令 (make) 的回傳值
time make -j"$CORE_NUM" 2>&1 | tee "$LOG_FILE"
MAKE_EXIT_CODE=${PIPESTATUS[0]}

# 檢查 make 是否成功
if [ $MAKE_EXIT_CODE -eq 0 ]; then
    # re make it
    time make
    echo "----------------------------------------"
    echo "編譯成功！(Exit Code: 0)"

    # --- 統計分析開始 ---

    # 1. 抓取包含 "警告：" 的原始行
    ALL_WARNINGS=$(grep "警告：" "$LOG_FILE")
    
    # 2. 分類：
    # REAL_WARNINGS: 排除 #warning，這通常是語法、變數未定義等真正的問題
    REAL_WARNINGS=$(echo "$ALL_WARNINGS" | grep -v "#warning")
    REAL_COUNT=$(echo "$REAL_WARNINGS" | grep -c "警告：" )
    
    # HASH_WARNINGS: 僅包含 #warning，這是開發者留下的提示
    HASH_WARNINGS=$(echo "$ALL_WARNINGS" | grep "#warning")
    HASH_COUNT=$(echo "$HASH_WARNINGS" | grep -c "警告：")

    # --- 顯示結果 ---

    # A. 顯示標準警告 (優先處理)
    if [ "$REAL_COUNT" -gt 0 ]; then
        echo -e "\033[33m[注意] 檢測到 $REAL_COUNT 個標準編譯器警告 (需檢查)：\033[0m"
        echo "----------------------------------------"
        echo "$REAL_WARNINGS"
        echo "----------------------------------------"
    else
        echo -e "\033[32m[完美] 沒有發現標準編譯器警告 (Clean Build)。\033[0m"
    fi

    # B. 顯示 #warning 數量 (新增功能)
    if [ "$HASH_COUNT" -gt 0 ]; then
        # 使用青色 (Cyan, 36) 顯示，區別於錯誤(紅)與警告(黃)
        echo -e "\033[36m[資訊] 另有 $HASH_COUNT 個預處理器提示 (#warning directives)。\033[0m"
        # 如果需要列出所有 #warning 內容，取消下面這行的註解：
        # echo "$HASH_WARNINGS"
    fi

else
    echo -e "\033[31m編譯失敗！\033[0m"
fi

date