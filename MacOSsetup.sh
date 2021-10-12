install_name_tool -change @rpath/libQt5Widgets.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtWidgets.framework/QtWidgets ./babel_client
install_name_tool -change @rpath/libQt5Gui.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtGui.framework/QtGui ./babel_client
install_name_tool -change @rpath/libQt5Network.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtNetwork.framework/QtNetwork ./babel_client
install_name_tool -change @rpath/libQt5Core.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtCore.framework/QtCore ./babel_client
install_name_tool -change @rpath/libQt5Sql.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtSql.framework/QtSql ./babel_client
install_name_tool -change @rpath/libQt5Test.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtTest.framework/QtTest ./babel_client
install_name_tool -change @rpath/libQt5PrintSupport.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtPrintSupport.framework/QtPrintSupport ./babel_client
install_name_tool -change @rpath/libQt5OpenGL.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtOpenGL.framework/QtOpenGL ./babel_client
install_name_tool -change @rpath/libQt5Concurrent.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtConcurrent.framework/QtConcurrent ./babel_client
install_name_tool -change @rpath/libQt5Xml.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtXml.framework/QtXml ./babel_client

# serv

install_name_tool -change @rpath/libQt5Widgets.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtWidgets.framework/QtWidgets ./babel_server
install_name_tool -change @rpath/libQt5Gui.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtGui.framework/QtGui ./babel_server
install_name_tool -change @rpath/libQt5Network.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtNetwork.framework/QtNetwork ./babel_server
install_name_tool -change @rpath/libQt5Core.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtCore.framework/QtCore ./babel_server
install_name_tool -change @rpath/libQt5Sql.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtSql.framework/QtSql ./babel_server
install_name_tool -change @rpath/libQt5Test.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtTest.framework/QtTest ./babel_server
install_name_tool -change @rpath/libQt5PrintSupport.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtPrintSupport.framework/QtPrintSupport ./babel_server
install_name_tool -change @rpath/libQt5OpenGL.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtOpenGL.framework/QtOpenGL ./babel_server
install_name_tool -change @rpath/libQt5Concurrent.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtConcurrent.framework/QtConcurrent ./babel_server
install_name_tool -change @rpath/libQt5Xml.5.dylib /Users/martin/Qt/5.15.2/clang_64/lib/QtXml.framework/QtXml ./babel_server