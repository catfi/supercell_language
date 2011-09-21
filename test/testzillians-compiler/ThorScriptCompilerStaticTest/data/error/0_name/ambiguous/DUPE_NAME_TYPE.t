class QWE {}

@static_test { expect_message={ level="LEVEL_ERROR", id="DUPE_NAME", parameters={ id="QWE" } } }
class QWE {}
