build_example(
  NAME node
  SOURCE_FILES node.cc
  LIBRARIES_TO_LINK
    ${libapplications}
    ${libcsma}
    ${libinternet}

)

build_example(
  NAME simple-red
  SOURCE_FILES simple-red.cc
  LIBRARIES_TO_LINK
    ${libapplications}
    ${libcsma}
    ${libinternet}
)

build_example(
  NAME red
  SOURCE_FILES red.cc
  LIBRARIES_TO_LINK
    ${libapplications}
    ${libcsma}
    ${libinternet}
    ${libflow-monitor}
)

build_example(
  NAME slow-red
  SOURCE_FILES slow-red.cc
  LIBRARIES_TO_LINK
    ${libapplications}
    ${libcsma}
    ${libinternet}
    ${libflow-monitor}
)

build_example(
  NAME red-cmd
  SOURCE_FILES red-cmd.cc
  LIBRARIES_TO_LINK
    ${libapplications}
    ${libcsma}
    ${libinternet}
    ${libflow-monitor}
)
