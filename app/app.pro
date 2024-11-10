QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++2a

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    MainWindow.cpp \
    circuits/AreaManager.cpp \
    circuits/CircuitCanvas.cpp \
    circuits/CircuitCanvasDnd.cpp \
    circuits/IdHandler.cpp \
    circuits/graph/DndBackup.cpp \
    circuits/graph/GraphDesigner.cpp \
    circuits/items/BaseCircuitItem.cpp \
    circuits/items/CircuitElement.cpp \
    circuits/items/CircuitInput.cpp \
    circuits/items/CircuitOutput.cpp \
    circuits/items/widgets/LogicVectorEdit.cpp \
    circuits/items/connectors/EndingConnector.cpp \
    circuits/items/connectors/StartingConnector.cpp \
    circuits/items/mime/BaseCircuitItemMimeData.cpp \
    circuits/items/mime/CircuitElementMimeData.cpp \
    circuits/items/mime/CircuitInputMimeData.cpp \
    circuits/items/mime/CircuitOutputMimeData.cpp \
    circuits/items/widgets/MultilineNumberEdit.cpp \
    dialogues/DialogCreateElementItem.cpp \
    dialogues/DialogCreateInputItem.cpp \
    dialogues/DialogCreateOutputItem.cpp \
    dialogues/DialogDuplicateElementItem.cpp \
    dialogues/ElementSizeChanger.cpp \
    logic/LogicController.cpp \
    logic/items/Connection.cpp \
    logic/items/ElementItem.cpp \
    logic/items/InputItem.cpp \
    logic/items/OutputItem.cpp \
    logic/simulation/QHDPipeController.cpp \
    logic/simulation/Simulator.cpp \
    main.cpp \
    presenters/CSVPresenter.cpp \
    presenters/SimulationPresenter.cpp \
    presenters/TableViewPresenter.cpp

HEADERS += \
    Config.hpp \
    ItemUtils.hpp \
    MainWindow.hpp \
    circuits/AreaManager.hpp \
    circuits/CircuitCanvas.hpp \
    circuits/IdHandler.hpp \
    circuits/graph/DndBackup.hpp \
    circuits/graph/CommonTypes.hpp \
    circuits/graph/GraphDesigner.hpp \
    circuits/items/BaseCircuitItem.hpp \
    circuits/items/CircuitElement.hpp \
    circuits/items/CircuitInput.hpp \
    circuits/items/CircuitOutput.hpp \
    circuits/items/widgets/LogicVectorEdit.hpp \
    circuits/items/connectors/EndingConnector.hpp \
    circuits/items/connectors/StartingConnector.hpp \
    circuits/items/mime/BaseCircuitItemMimeData.hpp \
    circuits/items/mime/CircuitElementMimeData.hpp \
    circuits/items/mime/CircuitInputMimeData.hpp \
    circuits/items/mime/CircuitOutputMimeData.hpp \
    circuits/items/widgets/MultilineNumberEdit.hpp \
    dialogues/DialogCreateElementItem.hpp \
    dialogues/DialogCreateInputItem.hpp \
    dialogues/DialogCreateOutputItem.hpp \
    dialogues/DialogDuplicateElementItem.hpp \
    dialogues/ElementSizeChanger.hpp \
    logic/LogicController.hpp \
    logic/items/AbstractItem.hpp \
    logic/items/Connection.hpp \
    logic/items/ElementItem.hpp \
    logic/items/InputItem.hpp \
    logic/items/OutputItem.hpp \
    logic/simulation/QHDPipeController.hpp \
    logic/simulation/QHDPipeController.hpp \
    logic/simulation/Simulator.hpp \
    logic/simulation/Simulator.hpp \
    presenters/CSVPresenter.hpp \
    presenters/PresenterTypes.hpp \
    presenters/SimulationPresenter.hpp \
    presenters/TableViewPresenter.hpp

FORMS += \
    MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
