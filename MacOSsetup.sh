install_name_tool -change @rpath/libQt5Widgets.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtWidgets.framework/QtWidgets ./bin/babel_client
install_name_tool -change @rpath/libQt5Gui.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtGui.framework/QtGui ./bin/babel_client
install_name_tool -change @rpath/libQt5Network.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtNetwork.framework/QtNetwork ./bin/babel_client
install_name_tool -change @rpath/libQt5Core.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtCore.framework/QtCore ./bin/babel_client
install_name_tool -change @rpath/libQt5Sql.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtSql.framework/QtSql ./bin/babel_client
install_name_tool -change @rpath/libQt5Test.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtTest.framework/QtTest ./bin/babel_client
install_name_tool -change @rpath/libQt5PrintSupport.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtPrintSupport.framework/QtPrintSupport ./bin/babel_client
install_name_tool -change @rpath/libQt5OpenGL.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtOpenGL.framework/QtOpenGL ./bin/babel_client
install_name_tool -change @rpath/libQt5Concurrent.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtConcurrent.framework/QtConcurrent ./bin/babel_client
install_name_tool -change @rpath/libQt5Xml.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtXml.framework/QtXml ./bin/babel_client

# serv

install_name_tool -change @rpath/libQt5Widgets.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtWidgets.framework/QtWidgets ./bin/babel_server
install_name_tool -change @rpath/libQt5Gui.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtGui.framework/QtGui ./bin/babel_server
install_name_tool -change @rpath/libQt5Network.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtNetwork.framework/QtNetwork ./bin/babel_server
install_name_tool -change @rpath/libQt5Core.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtCore.framework/QtCore ./bin/babel_server
install_name_tool -change @rpath/libQt5Sql.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtSql.framework/QtSql ./bin/babel_server
install_name_tool -change @rpath/libQt5Test.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtTest.framework/QtTest ./bin/babel_server
install_name_tool -change @rpath/libQt5PrintSupport.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtPrintSupport.framework/QtPrintSupport ./bin/babel_server
install_name_tool -change @rpath/libQt5OpenGL.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtOpenGL.framework/QtOpenGL ./bin/babel_server
install_name_tool -change @rpath/libQt5Concurrent.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtConcurrent.framework/QtConcurrent ./bin/babel_server
install_name_tool -change @rpath/libQt5Xml.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtXml.framework/QtXml ./bin/babel_server