// Copyright 2011 Jérôme Velut. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
//    1. Redistributions of source code must retain the above copyright notice, this list of
//       conditions and the following disclaimer.
//
//    2. Redistributions in binary form must reproduce the above copyright notice, this list
//       of conditions and the following disclaimer in the documentation and/or other materials
//       provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY Jérôme Velut ``AS IS'' AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
// FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Jérôme Velut OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// The views and conclusions contained in the software and documentation are those of the
// authors and should not be interpreted as representing official policies, either expressed
// or implied, of Jérôme Velut.
//
// ORIGINAL HOME PAGE: https://github.com/jeromevelut/yaap

#ifndef __yaap_h__
#define __yaap_h__

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

//! \namespace yaap contains the classes for command line arguments parsing
namespace yaap {

const int undef = 0;

//! \class Option
//! \brief Defines a boolean option
//!
//! An option is defined by its flag, a description and a state. The flag and
//! and description are given at construction, whereas the state is defined
//! after the effective parsing.
class Option {
public:
    //! constructor.
    Option( char flag, std::string description )
    {
        this->flag = flag;
        this->description = description;
        this->state = false;
        this->required = false;
        this->error = false;
    };

    //! Get the flag of this option.
    char Flag( ) {
        return( this->flag);
    };
    //! Set the state (exists or not in the command line)
    void Exists(bool state) {
        this->state = state;
    };
    //! Get the state. If true, the option exists in the command line
    bool Exists() {
        return(this->state);
    };

    //! Set the requirement level (required is true, optional is false)
    void SetRequired( bool r ) {
        this->required = r;
    };

    //! Set the error flag to true
    void RaiseError( ) {
        this->error =true;
    };

    //! Get the error state
    bool ErrorFlag() {
        return( this->error );
    };

    bool IsRequired( ) {
        return( this->required );
    };

    //! Print how to use the option in the command line format
    virtual void CLUsage( )
    {
        std::cout << " [-" << this->Flag() << "]";
    };

    std::string GetDescription( ) {
        return( this->description );
    };

protected:
    char flag; //!< Command line flag character
    std::string description;//!< Short description
    bool state;//!< true if present in the command line
    bool required;//!< if true, the absence of the option in the command line will raise an error in the parser.
    bool error; //!< if true, an error occured while parsing.
};

//! \class OptionArg
//! \brief Defines an option with possibly several arguments
//!
//! This is a templated class derived from Option. It adds a vector of
//! arguments which type and number is defined by the template statements T
//! and N.
template<typename T>
class OptionArg : public Option {

public:
    //! constructor
    OptionArg( char flag, std::string description, unsigned int nbargs = yaap::undef ):Option(flag,description)
    {
        this->nbArgs = nbargs;
    };

    //! Add an argument at the end of the arg list
    void AddArgument( std::istringstream& streamArg );

    //! Get the pos-th arg of type T
    T GetArgument( unsigned int pos ) {
        return( argVector[pos] );
    };

    //! Convenience function for 1-subarg argument
    T GetValue( ) {
        return( argVector[0] );
    };

    //! Print how to use the option in the command line format
    virtual void CLUsage( )
    {
        std::cout << " [-" << this->Flag();
        if( this->nbArgs == yaap::undef )
            std::cout << " x1 x2 ...";
        else
            for( unsigned int i = 0; i < nbArgs; i++ )
                std::cout << " x";
        std::cout << "]";
    };

protected:
    unsigned int nbArgs; //!< Number of arguments of this specific option
    std::vector<T> argVector; //!< Vector of arguments of type T
};

//! Template specialization of SetArgument - general definition
template<typename T>
void OptionArg<T>::AddArgument( std::istringstream& streamArg )
{
    T arg;
    streamArg >> arg;
    if( streamArg.fail())
    {
        this->RaiseError();
    }

    this->argVector.push_back( arg );
}

//! Template specialization of SetArgument - istd::string definition
template<>
void OptionArg<std::string>::AddArgument( std::istringstream& streamArg )
{
    std::string arg;
    std::getline(streamArg, arg);

    if( streamArg.fail())
    {
        this->RaiseError();
    }

    this->argVector.push_back( arg );
}

//! Template specialization of SetArgument - unsigned int definition
//! Uint data should be passed as hexadecimal, prefix '0x'
template<>
void OptionArg<unsigned int>::AddArgument( std::istringstream& streamArg )
{
    std::string stringArg;
    streamArg >> stringArg;

    bool hexa = false;
    if( stringArg.find("0x") == 0 ) // hexa prefix has been found at first position
    {
        hexa = true;
        stringArg = stringArg.substr(2);
    }

    std::istringstream newStream( stringArg ); // new stream with argument without prefix (if any)

    unsigned int arg;
    if( hexa == true )
        newStream >> std::hex >> arg;
    else
        newStream >> arg;

    if( newStream.fail())
    {
        this->RaiseError();
    }

    this->argVector.push_back( arg );
}

//! \class OperandBase
//! \brief define a command line operand
//! 
//! This is a bare base class intended to be used as std::vector element. 
//! \see Operand<T>
class OperandBase{
public:
    OperandBase( const std::string& description){
        this->description = description;
    };
    virtual ~OperandBase(){};
private:
    std::string description; //!< short description of the operand's role
};    

//! \class Operand
//! \brief Implement a defined type for a command line operand
template<typename T>
class Operand : public OperandBase{
public:
    Operand( const std::string& description ):OperandBase( description ){
    };

    void SetValue( std::istringstream& valStream );
    

    T GetValue( ){
        return( this->value );
    };

private:
    T value;
};

//! Template specialization for string operand (white space hack)
template<typename T>
void Operand<T>::SetValue( std::istringstream& valStream )
{
    valStream >> this->value;
}

//! Template specialization for string operand (white space hack)
template<>
void Operand<std::string>::SetValue( std::istringstream& valStream )
{
    std::getline( valStream, this->value );
}
//! \class Parser
//! \brief Manages a set of options
class Parser {
public:
    //! constructor. Initializes number of args and argument vector.
    Parser( int argc, char** argv, std::string description = "" )
    {
        this->nbArgs = argc;
        this->argv = argv;
        this->error = false;
        this->description = description;
  
        this->operandOffset = 1; // Case with no option
        // find the first possible operand (use of "--" flag)
        for( int argId = 0; argId < argc; argId++ )
        {
            if( argv[argId][0] == '-' )
                if( argv[argId][1] == '-' )
                    if( argv[argId][2] == '\0' )
                    {
                        this->operandOffset = argId+1;
                    }
        }
    };

    //! destructor
    virtual ~Parser()
    {
        // todo: delete options
        std::vector<Option*>::iterator optionIterator = this->optionVector.begin(); 
        while( optionIterator != this->optionVector.end() )
        {
            delete *optionIterator;
            optionIterator++;
        }
    };

    //! Add a simple option with given flag and description to the options
    //! vector and check its existence.
    //! \return the instanciated Option
    Option* AddOption( char flag, std::string description, bool required = false )
    {
        Option* option = new Option( flag, description );
        option->SetRequired( required );
        // For each argument in the command, check the underlying string
        for( unsigned int i = 1; i < this->nbArgs; i++ )
        {
            if( this->argv[i][0] == '-' ) // This is an option(s) statement
            {
                // The option flag can be concatenated after a unique '-'
                unsigned int c = 0;
                while(argv[i][c] != '\0' )
                {
                    if( argv[i][c] == option->Flag() ) // It is the processed option
                        option->Exists(true); // toggle the state in the Option object
                    c++;
                }
            }
        }
        // Put the Option in the options' array.
        this->PushOption( option );
        // if required but not found, raise an error
        if( !option->Exists() && required )
        {
            option->RaiseError( );
            this->error = true;
        }
        // Return the created Option for the user to use it in the main program
        return( option );
    };

    //! Add an option with arguments with given flag and description to the
    //! options vector, check its existence and perform the sub-arguments
    //! assignment. There are nsubargs of type T.
    //! \return the instanciated OptionArg
    template<class T>
    OptionArg<T>* AddOptionArg( char flag, std::string description, unsigned int nbsubargs, bool required = false )
    {
        // option allocation
        OptionArg<T>* option = new OptionArg<T>( flag, description, nbsubargs );
        option->SetRequired( required);

        // parse the argument list
        for( unsigned int i = 1; i < this->nbArgs; i++ )
        {
            if( this->argv[i][0] == '-' ) // This is an option(s) statement
            {
                if( argv[i][1] == option->Flag() ) // this is the processed option
                {
                    // toggle the state of the option to true
                    option->Exists(true);
                    if( i + nbsubargs >= this->nbArgs )
                    {
                        option->RaiseError();
                        this->error = true;
                    }
                    else
                        for( unsigned int argIdx = 1; argIdx <= nbsubargs ; argIdx++)
                        {
                            // For each sub-argument, memorize the command line value in
                            // the OptionArg object
                            std::istringstream argStream( argv[i+argIdx] );
                            option->AddArgument( argStream );
                            if( option->ErrorFlag() )
                                this->error = true;
                        }
                }
            }
        }
        // Put the Option in the options' array.
        this->PushOption( option );
        // if required but not found, raise an error
        if( !option->Exists() && required )
        {
            this->error = true;
            option->RaiseError();
        }
        // Return the created Option for the user to use it in the main program
        return( option );
    };

    template<typename T>
    Operand<T>* AddOperand( std::string description ){

        Operand<T>* op = new Operand<T>(description);

        if( this->operandOffset >= this->nbArgs )
        {
            this->error = true;
            std::istringstream nullStream( "0" );
            op->SetValue( nullStream );
        }
        else
        {
            std::istringstream opStream( this->argv[this->operandOffset] );
            op->SetValue( opStream );
        }
        this->operandVector.push_back( op );
        this->operandOffset++;
        return( op );
    };

    OperandBase* GetOperand( unsigned int pos ){
        return( this->operandVector[pos] );
    };

    void Usage( )
    {
        std::cout <<std::endl<< "Utility " << this->argv[0] << " :" << std::endl;
        std::cout << std::endl<<this->description << std::endl;
        std::cout << std::endl<<"Usage: \n [shell]$ " << this->argv[0];
        for( unsigned int i = 0; i < optionVector.size(); i++ )
            optionVector[i]->CLUsage();
        std::cout << std::endl;

        for( unsigned int i = 0; i < optionVector.size(); i++ )
        {
            std::string requirement;
            if( optionVector[i]->IsRequired( ))
                requirement = "Required";
            else
                requirement = "Optional";

            if( optionVector[i]->ErrorFlag() )
                std::cout << "     *\t";
            else
                std::cout << "\t";
            std::cout << "-" << optionVector[i]->Flag()
                      << " : " << optionVector[i]->GetDescription()
                      << " ("<<requirement<<")."<<std::endl;
        }
        std::cout << "* indicate(s) wrong argument(s)."<< std::endl;
    };

    bool IsCommandLineValid( )
    {
        return( !this->error );
    };

    void SetDescription( std::string desc )
    {
        this->description = desc;
    };

private:
    void PushOption( Option* option ){
        this->optionVector.push_back( option );
        if( // Reserved POSIX flags
            option->Flag() == 'W'
         || option->Flag() == '-'
          ) 
        {
            option->RaiseError( );
            this->error = true;
        }
    };


    unsigned int nbArgs; //!< Number of arguments (argc)
    char** argv; //!< Arguments' vector
    std::vector<Option*> optionVector; //!< vector of options
    std::vector<OperandBase*> operandVector; //!< vector of options
    unsigned int operandOffset; //!< index in argv of the first operand
    bool error; //!< raised to 1 when one of the arguments in the command line is not valid
    std::string description; //!< give a general description of the command.
};


}; //end namespace yaap

#endif //yaap
