#include "window.h"

struct Sheet *mouseSheet;
struct WindowsManager windowsManager;

void initWindowsManager()
{
    int i;
    for (i = 0; i < ACTIVE_SHEET_ITEM_TOTAL; i++)
    {
        windowsManager.activeSheetItemStore[i].isUsing = false;
    }
}

void deleteWindowsManagerActiveSheetItems()
{
    struct ActiveSheetItem *currentItem = windowsManager.firstActiveSheet;
    while (currentItem != NULL)
    {
        struct ActiveSheetItem *nextItem = currentItem->nextSheet;
        removeActiveSheetItem(currentItem);
        currentItem = nextItem;
    }
    windowsManager.firstActiveSheet = NULL;
    windowsManager.lastActiveSheet = NULL;
}

struct ActiveSheetItem *getActiveSheetItem()
{
    int i;
    for (i = 0; i < ACTIVE_SHEET_ITEM_TOTAL; i++)
    {
        if (windowsManager.activeSheetItemStore[i].isUsing == false)
        {
            windowsManager.activeSheetItemStore[i].isUsing = true;
            return &windowsManager.activeSheetItemStore[i];
        }
    }

    return NULL;
}

struct ActiveSheetItem *removeActiveSheetItem(struct ActiveSheetItem *item)
{
    item->previousSheet = NULL;
    item->nextSheet = NULL;
    item->sheet = NULL;
    item->isUsing = false;
}

void initMouseCursorSheet(struct Sheet *rootSheet)
{
    // mouseSheet = createSubsheetToTop(rootSheet, 0, 0, rootSheet->width, rootSheet->height);
    mouseSheet = createSubsheetToTop(rootSheet, 0, 0, MOUSE_WIDTH, MOUSE_HEIGHT);
    rootSheetManager.mouseSheet = mouseSheet;
    setFixedTop(mouseSheet);

    fillInSheet(mouseSheet, 0, 0, MOUSE_WIDTH, MOUSE_HEIGHT, COL_TRANSPARENT);
    initMouseCursor(mouseSheet, 0, 0);
    // printInSheet(mouseSheet, 0, 32, "123", COL8_FFFFFF);

    mouseSheet->attribute[0] = 0; //x
    mouseSheet->attribute[1] = 0; //y
    forceUpdateSheet(mouseSheet);
}

void updateMouseCursorSheet(short moveX, short moveY)
{
    // fillInSheet(mouseSheet, mouseSheet->attribute[0], mouseSheet->attribute[1], MOUSE_WIDTH, MOUSE_HEIGHT, COL_TRANSPARENT);
    mouseSheet->attribute[0] += moveX;
    mouseSheet->attribute[1] += moveY;

    int currentIndex = mouseSheet->fatherSheet->indexMap[mouseSheet->attribute[1] * getBootInfo()->screenX + mouseSheet->attribute[0]];
    if (currentIndex != mouseSheet->index)
    {
        mouseData.rootIndexOfPointer = currentIndex;
    }
    else
    {
        currentIndex = mouseData.rootIndexOfPointer;
    }

    if (mouseSheet->attribute[0] < 0)
    {
        mouseSheet->attribute[0] = 0;
    }
    else if (mouseSheet->attribute[0] >= getBootInfo()->screenX)
    {
        mouseSheet->attribute[0] = getBootInfo()->screenX - 1;
    }

    if (mouseSheet->attribute[1] < 0)
    {
        mouseSheet->attribute[1] = 0;
    }
    else if (mouseSheet->attribute[1] >= getBootInfo()->screenY)
    {
        mouseSheet->attribute[1] = getBootInfo()->screenY - 1;
    }
    // initMouseCursor(mouseSheet, mouseSheet->attribute[0], mouseSheet->attribute[1]);
    moveSheet(mouseSheet, mouseSheet->attribute[0], mouseSheet->attribute[1]);
    mouseData.preX = mouseData.x;
    mouseData.preY = mouseData.y;
    mouseData.x = mouseSheet->attribute[0];
    mouseData.y = mouseSheet->attribute[1];

    // updateAllSubsheet(mouseSheet->fatherSheet);

    // forceUpdateSheet(mouseSheet);
    // updateSheet(mouseSheet);
    // fillVramByIndexMap(mouseSheet->fatherSheet);
    // updateSheet(mouseSheet);

    // char c[32];
    // fillInSheet(mouseSheet->fatherSheet->bottomSheet, 0, 80, 150, 16, COL8_000000);
    // sprintf(c, "(%d,%d) %d", mouseSheet->attribute[0], mouseSheet->attribute[1], currentIndex);
    // printInSheet(mouseSheet->fatherSheet->bottomSheet, 0, 80, c, COL8_FFFFFF);
}

void initDesktop(struct Sheet *rootSheet)
{
    // struct Sheet *desktopSheet = createSubsheetToTop(rootSheet, 0, 0, rootSheet->width, rootSheet->height);
    struct Sheet *desktopSheet = createSubsheetToTop(rootSheet, 0, 0, rootSheet->width, 18);
    rootSheetManager.desktopSheet = desktopSheet;
    // struct Sheet *desktopSheet = createSubsheetToTop(rootSheet, 0, 0, rootSheet->width, 18);

    setFixedTop(desktopSheet);

    struct Sheet *desktopStatusSheet = createSubsheetToTop(desktopSheet, 0, 0, desktopSheet->width, 17);
    struct Sheet *desktopStatusSheetBackground = createSubsheetToTop(desktopStatusSheet, 0, 0, desktopStatusSheet->width, desktopStatusSheet->height);
    fillVram(desktopStatusSheetBackground, COL8_FFFFFF);
    setFixedBottom(desktopStatusSheetBackground);

    fillVram(desktopStatusSheet, COL8_FFFFFF);
    updateSheet(desktopStatusSheet);

    // fillInSheet(desktopSheet, 0, 0, desktopSheet->width, 17, COL8_FFFFFF);

    fillInSheet(desktopSheet, 0, 17, desktopSheet->width, 1, COL8_848484);
    // fillInSheet(desktopSheet, 0, 18, desktopSheet->width, desktopSheet->height - 18, COL_TRANSPARENT);

    // char s[64];
    // sprintf(s, "%d %uMB", desktopSheet->nextSheet->index, getUnusedMemoryTotal(getMemoryManager()) / (1024 * 1024));
    struct Sheet *titleSheet = createLabel(desktopStatusSheet, 8, 0, 128, 16, "Desktop", COL8_000000);
    rootSheetManager.titleLabel = titleSheet;
    // printInSheet(desktopSheet, 8, 0, "Explorer", COL8_000000);

    // printInSheet(desktopSheet, 8, 0, s, COL8_000000);

    updateSheet(desktopSheet);
    // updateSheet(titleSheet);
}

struct Window *createWindow(struct Sheet *fatherSheet, short x, short y, short width, short height, char *title)
{

    struct Window *window = allocaMemory(getMemoryManager(), sizeof(struct Window));

    struct Sheet *windowSheet = createSubsheetToTop(fatherSheet, x, y, width, height);
    window->sheet = windowSheet;
    windowSheet->fatherWindow = window;

    //背景图层
    struct Sheet *backgroundSheet = createSubsheetToTop(windowSheet, 0, 0, width, height);
    window->backgroundSheet = backgroundSheet;
    backgroundSheet->fatherWindow = window;

    fillInSheet(backgroundSheet, 0, 0, width, height, COL8_FFFFFF);
    setFixedBottom(backgroundSheet);

    //状态图层
    struct Sheet *statusSheet = createSubsheetToTop(windowSheet, 0, 0, width, height);
    window->statusBarSheet = statusSheet;
    statusSheet->fatherWindow = window;

    fillInSheet(statusSheet, 1, 19, width - 2, height - 19, COL_TRANSPARENT);
    fillInSheet(statusSheet, 1, 1, width - 2, 17, COL8_FFFFFF);
    fillInSheet(statusSheet, 0, 0, width, 1, COL8_848400);
    fillInSheet(statusSheet, 0, 0, 1, height, COL8_848484);
    fillInSheet(statusSheet, width - 1, 0, 1, height, COL8_848484);
    fillInSheet(statusSheet, 0, height - 1, width, 1, COL8_848484);
    fillInSheet(statusSheet, 0, 18, width, 1, COL8_848484);

    initFourRadius(statusSheet);

    //三个按钮
    struct Sheet *buttonSheet = createSubsheetToTop(statusSheet, 10, 6, 30, 8);
    struct Sheet *backgroundOfButtonSheet = createSubsheetToTop(buttonSheet, 0, 0, 30, 8);
    window->buttonSheet = buttonSheet;
    window->backgroundOfButtonSheet = backgroundOfButtonSheet;
    buttonSheet->fatherWindow = window;
    backgroundOfButtonSheet->fatherWindow = window;
    fillVram(backgroundOfButtonSheet, COL8_FFFFFF);
    setFixedBottom(backgroundOfButtonSheet);

    struct Sheet *closeBtn = createSubsheetToTop(buttonSheet, 0, 0, 8, 8);
    window->closeButtonSheet = closeBtn;
    closeBtn->fatherWindow = window;
    initButtonCircle(closeBtn, 0, 0, COL8_FF0000);
    updateSheet(closeBtn);

    setFixedTop(statusSheet);
    updateIndexMapAndActionMap(windowSheet);
    fillVramByIndexMap(windowSheet);

    //标题
    int labelX = width / 2 - getStringSize(title) * 8 / 2;
    struct Sheet *titleSheet;
    //45
    if (labelX <= 25)
    {
        labelX = 25;
        // title = "...";
        titleSheet = createLabelWithBackground(statusSheet, labelX, 2, getStringSize("...") * 8, 16, "...", COL8_000000, COL8_FFFFFF);
    }
    else
    {
        titleSheet = createLabelWithBackground(statusSheet, labelX, 2, getStringSize(title) * 8, 16, title, COL8_000000, COL8_FFFFFF);
    }
    window->titleSheet = titleSheet;
    window->title = title;

    titleSheet->fatherWindow = window;
    // updateSheet(windowSheet);

    //添加事件
    window->sheet->systemActionManager = allocaMemory(getMemoryManager(), sizeof(struct ActionManager));
    window->statusBarSheet->systemActionManager = allocaMemory(getMemoryManager(), sizeof(struct ActionManager));
    window->closeButtonSheet->systemActionManager = allocaMemory(getMemoryManager(), sizeof(struct ActionManager));

    window->sheet->userActionManager = allocaMemory(getMemoryManager(), sizeof(struct ActionManager));

    window->sheet->systemActionManager->onClick = &activeSheetWindow;
    window->sheet->systemActionManager->onKeyPress = &onWindowKeyPress;
    window->sheet->systemActionManager->onKeyUp = &onWindowKeyUp;

    window->statusBarSheet->systemActionManager->onClick = &onWindowStatusBarClick;
    window->statusBarSheet->systemActionManager->onMouseLeftDown = &onWindowStatusBarMouseLeftDown;

    window->closeButtonSheet->systemActionManager->onMouseLeftDown = &onWindowCloseButtonMouseDown;
    window->closeButtonSheet->systemActionManager->onMouseLeftUp = &onWindowCloseButtonMouseUp;

    activeSheetWindow(window->sheet);

    return window;
}

void onWindowStatusBarMouseLeftDown(struct Sheet *this, unsigned int x, unsigned int y)
{
    windowsManager.isDragging = true;
}

void onWindowStatusBarClick(struct Sheet *this, unsigned int x, unsigned int y)
{
    if (windowsManager.isDragging)
    {
        int moveX = mouseData.x - mouseData.preX;
        int moveY = mouseData.y - mouseData.preY;

        moveSheet(this->fatherSheet, this->fatherSheet->x + moveX, this->fatherSheet->y + moveY);
    }
    // else
    // {
    //     windowsManager.isDragging = true;
    // }
}

void activeSheetWindow(struct Sheet *sheet)
{
    activeWindow(sheet->fatherWindow);
    moveSheetToTop(sheet->fatherWindow->sheet);
    setLabelText(rootSheetManager.titleLabel, sheet->fatherWindow->title, COL8_000000);
}

void disactiveSheetWindow(struct Sheet *sheet)
{
    disactiveWindow(sheet->fatherWindow);
}

void activeWindow(struct Window *window)
{
    struct Window *previousActiveWindow = windowsManager.currentActiveWindow;
    if (previousActiveWindow == window && windowsManager.isActiveShowing == true)
    {
        return;
    }

    windowsManager.isActiveShowing = true;
    if (previousActiveWindow != NULL)
    {
        disactiveWindow(previousActiveWindow);
    }

    windowsManager.currentActiveWindow = window;
    fillWindowBackground(window, COL8_FFFFFF);
}

void disactiveWindow(struct Window *window)
{
    fillWindowBackground(window, COL8_848484);
}

void fillWindowBackground(struct Window *window, unsigned int color)
{
    fillVram(window->backgroundOfButtonSheet, color);

    struct Window *statusSheet = window->statusBarSheet;
    unsigned int width = window->sheet->width;
    unsigned int height = window->sheet->height;
    fillInSheet(statusSheet, 1, 19, width - 2, height - 19, COL_TRANSPARENT);
    fillInSheet(statusSheet, 1, 1, width - 2, 17, color);
    fillInSheet(statusSheet, 0, 0, width, 1, COL8_848400);
    fillInSheet(statusSheet, 0, 0, 1, height, COL8_848484);
    fillInSheet(statusSheet, width - 1, 0, 1, height, COL8_848484);
    fillInSheet(statusSheet, 0, height - 1, width, 1, COL8_848484);
    fillInSheet(statusSheet, 0, 18, width, 1, COL8_848484);
    initFourRadius(statusSheet);

    fillVram(window->backgroundSheet, color);

    window->titleSheet->attribute[0] = color;
    int labelX = width / 2 - getStringSize(window->title) * 8 / 2;
    if (labelX <= 25)
    {
        setLabelText(window->titleSheet, "...", COL8_000000);
    }
    else
    {
        setLabelText(window->titleSheet, window->title, COL8_000000);
    }
    // setLabelText(window->titleSheet, window->title, COL8_000000);

    updateIndexMapAndActionMap(statusSheet);
    fillVramByIndexMap(statusSheet);
    // updateSheet(window->titleSheet);
    // updateIndexMapAndActionMap(window->backgroundOfButtonSheet);
    // fillVramByIndexMap(window->backgroundOfButtonSheet);
    updateSheet(window->backgroundOfButtonSheet);
    // updateSheet(statusSheet);
}

void onWindowCloseButtonMouseDown(struct Sheet *this)
{
    initButtonCircle(this->fatherWindow->closeButtonSheet, 0, 0, COL8_848484);
    updateSheet(this->fatherWindow->closeButtonSheet);
}

void onWindowCloseButtonMouseUp(struct Sheet *this, unsigned int x, unsigned int y)
{
    initButtonCircle(this->fatherWindow->closeButtonSheet, 0, 0, COL8_FF0000);
    updateSheet(this->fatherWindow->closeButtonSheet);

    if (this->fatherWindow->closeButtonSheet->userActionManager != NULL &&
        this->fatherWindow->closeButtonSheet->userActionManager->onClick != NULL)
    {
        this->fatherWindow->closeButtonSheet->userActionManager->onClick(this, x, y);
    }

    //关闭窗体
    releaseWindow(this->fatherWindow);
}

void onWindowKeyPress(struct Sheet *this, char c, unsigned int raw)
{
    if (this->userActionManager != NULL && this->userActionManager->onKeyPress != NULL)
    {
        this->userActionManager->onKeyPress(this, c, raw);
    }
}

void onWindowKeyUp(struct Sheet *this, char c, unsigned int raw)
{
    if (this->userActionManager != NULL && this->userActionManager->onKeyUp != NULL)
    {
        this->userActionManager->onKeyUp(this, c, raw);
    }
}

void releaseWindow(struct Window *window)
{
    struct Sheet *sheet = window->sheet;

    sheet->previousSheet->nextSheet = sheet->nextSheet;
    sheet->nextSheet->previousSheet = sheet->previousSheet;
    updateIndexMapAndActionMap(sheet->fatherSheet);
    fillVramByIndexMap(sheet->fatherSheet);
    updateSheet(sheet->fatherSheet);

    releaseSheetRecursive(sheet->topSheet);
    releaseSingleSheet(sheet);

    releaseBlock(getMemoryManager, window->title);
    releaseBlock(getMemoryManager(), window);
}

void releaseSheetRecursive(struct Sheet *sheet)
{
    if (sheet->topSheet != NULL)
    {
        releaseSheetRecursive(sheet->topSheet);
    }

    if (sheet->nextSheet != NULL)
    {
        releaseSheetRecursive(sheet->nextSheet);
    }

    releaseSingleSheet(sheet);
}

void releaseSingleSheet(struct Sheet *temp)
{
    releaseBlock(getMemoryManager(), temp->vram);
    releaseBlock(getMemoryManager(), temp->actionMap);
    releaseBlock(getMemoryManager(), temp->indexMap);
    releaseBlock(getMemoryManager(), temp->updateMap);

    if (temp->systemActionManager != NULL)
    {
        releaseBlock(getMemoryManager(), temp->systemActionManager);
    }

    if (temp->userActionManager != NULL)
    {
        releaseBlock(getMemoryManager(), temp->userActionManager);
    }

    if (temp->attribute[5] == BUTTON)
    {
        struct Button *button = temp->attribute[2];
        releaseBlock(getMemoryManager(), button->title);
        releaseBlock(getMemoryManager(), button);
    }

    if (temp->attribute[5] == TEXTFIELD)
    {
        struct TextField *textField = temp->attribute[2];
        releaseBlock(getMemoryManager(), textField->content);
        releaseBlock(getMemoryManager(), textField->displayContent);
        releaseBlock(getMemoryManager(), textField);
    }

    releaseBlock(getMemoryManager(), temp);
}