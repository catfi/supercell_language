#include "language/grammar/WhiteSpace.h"
#include "language/grammar/ThorScriptPackageDependencyGrammar.h"

namespace zillians { namespace language { namespace grammar {

template<typename Iterator>                                                                                                                                                                                        
bool getImportedPackagesTImpl(Iterator first, Iterator end, std::vector<std::wstring>& v)                                                                                                                              
{                                                                                                                                                                                                                  
    bool r = qi::phrase_parse(first, end,                                                                                                                                                                         
            // Begin grammar                                                                                                                                                                                       
            (                                                                                                                                                                                                      
                  -( (L"package" > *(boost::spirit::unicode::char_ - ';') > L';') )                                                                                                                                
                > *( (L"import"  > boost::spirit::as_wstring[*unicode::char_(L"0-9a-zA-Z_.")][boost::phoenix::push_back(boost::phoenix::ref(v), boost::spirit::qi::_1)] > L';') )                                         
                > *( boost::spirit::unicode::char_ )
                > qi::eoi
            )                                                                                                                                                                                                      
            ,                                                                                                                                                                                                      
            // End grammar                                                                                                                                                                                         
            detail::WhiteSpace<Iterator>());                                                                                                                                                                                   
    return r ;                                                                                                                                                                                                     
}

bool getImportedPackages(pos_iterator_type begin, pos_iterator_type end, std::vector<std::wstring>& v)
{
    return getImportedPackagesTImpl(begin, end, v);
}

} } } // zillians language grammar
