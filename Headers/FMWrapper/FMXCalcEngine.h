/*
 
 Copyright © 1998 - 2017  FileMaker, Inc.
 All rights reserved.
 
 FileMaker, Inc. grants you a non-exclusive limited license to use this file solely to enable
 licensees of FileMaker Pro Advanced to compile plug-ins for use with FileMaker products.
 Redistribution and use in source and binary forms, without modification, are permitted provided
 that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this list of
 conditions and the following disclaimer.
 
 * The name FileMaker, Inc. may not be used to endorse or promote products derived from
 this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY FILEMAKER, INC. ''AS IS'' AND ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL FILEMAKER, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 */

#ifndef _h_DBCalcEngine_
#define _h_DBCalcEngine_

#include "FMWrapper/FMXClient.h"

extern "C++" 
{
    namespace fmx
    {
        class Text;
        class FixPt;
        class DateTime;
        class BinaryData;
        class Data;
        class DataVect;
        class ExprEnv;

        class DataVect
        {
        public:
            inline uint32 Size () const;
            inline void Clear ();
            inline bool IsEmpty () const;
            inline void PushBack ( const Data &data );
            inline Data *PopBack ();
            inline const Data &At ( uint32 position ) const;
            inline const Text &AtAsText ( uint32 position ) const;
            inline const FixPt &AtAsNumber ( uint32 position ) const;
            inline const DateTime &AtAsDate ( uint32 position ) const;
            inline const DateTime &AtAsTime ( uint32 position ) const;
            inline const DateTime &AtAsTimeStamp ( uint32 position ) const;
            inline bool AtAsBoolean ( uint32 position ) const;
            inline const BinaryData &AtAsBinaryData ( uint32 position ) const;
            inline void operator delete ( void *obj );

        private:
            DataVect ();
            DataVect ( const DataVect& other );

        };

        class RowVect
        {
        public:
            inline uint32 Size () const;
            inline bool IsEmpty () const;
            inline const DataVect &At ( uint32 position ) const;
            inline void operator delete ( void *obj );

        private:
            RowVect ();
            RowVect ( const RowVect& other );

        };

        // Calculation and script step function definition
        FMX_PROCPTR( errcode, ExtPluginType ) ( short functionId, const ExprEnv& env, const DataVect& parms, Data& result );
        
        class ExprEnv
        {
        public:
            enum
            {
                // DEPRECATED in FileMaker Pro 12. The server database process no longer loads plug-ins. Stored calculations
                // containing plug-in functions will not evaluate the same way they do on a client with the plug-in if a
                // schema change forces the server to recalculate the stored calculation's value.
                kMayEvaluateOnServer    = 0x00000001,

                // DEPRECATED in FileMaker Pro 16. All the following flags will be ignored for the commented purpose in version
                // 16 and later. Note the change of kDisplayInAllDialogs further below.
                kDisplayCalcFields      = 0x00000100,   // Calculated fields
                kDisplayAutoEnter       = 0x00000200,   // Auto enter calculation
                kDisplayValidation      = 0x00000400,   // Validation by calculation
                kDisplayCustomFunctions = 0x00000800,   // Custom function definition
                kDisplayPrivileges      = 0x00001000,   // Row level access calculations
                kDisplayInFuture1       = 0x00002000,   // As yet undefined calculation dialog
                kDisplayInFuture2       = 0x00004000,   // As yet undefined calculation dialog
                kDisplayGeneric         = 0x00008000,   // Dialog used by scripting and replace

                // Changed in FileMaker Pro 16v2. If any of these bits are set, the function will be displayed in any picking
                // UI control. Future functions should use this constant if they want to be displayed. The function can always
                // be typed in manually.
                kDisplayInAllDialogs    = 0x0000FF00,

                // New to FileMaker Pro 16 (API VERSION 57) and later. Bits used in the renamed/new compatibleOnFlags parameter. If
                // all the kFutureCompatible bits are zero then it is treated as if all the kFutureCompatible bits were set.
                kMacCompatible          = 0x00000002,
                kWinCompatible          = 0x00000004,
                kServerCompatible       = 0x00000008,
                kIOSCompatible          = 0x00000010,
                kCustomWebCompatible    = 0x00000020,
                kWebDirectCompatible    = 0x00000040,
                kAllDeviceCompatible    = kMacCompatible | kWinCompatible | kServerCompatible | kIOSCompatible | kCustomWebCompatible | kWebDirectCompatible,
                kFutureCompatible       = kAllDeviceCompatible | 0x00FF0000,

                // New to FileMaker Pro 16 (API VERSION 57) and later. If one of these errcodes are returned by either a calc or
                // script step function call, the results will be used to set the value of the Get(LastExternalErrorDetail) calc.
                kPluginErrResult1       = 1552,
                kPluginErrResult2       = 1553,
                kPluginErrResult3       = 1554,
                kPluginErrResult4       = 1555,
                kPluginErrResult5       = 1556,
                kPluginErrResult6       = 1557,
                kPluginErrResult7       = 1558,
                kPluginErrResult8       = 1559
            };

            inline errcode Evaluate ( const Text &expression, Data &result ) const;

            inline errcode ExecuteFileSQL ( const Text &expression, const Text &filename, const DataVect &parameters, RowVect& result ) const;

            inline void operator delete ( void *obj );

            // New to FileMaker Pro 13 (API VERSION 54) and later
            // This acts much like the previous deprecated and removed ExecuteSQL method but uses the more correct/strict SQL
            // syntax of ExecuteFileSQL
            inline errcode ExecuteFileSQLTextResult ( const Text &expression, const Text &filename, const DataVect &parameters,
                                                      Data &result, uint16 colSep, uint16 rowSep ) const;

            // New to FileMaker Pro 15 (API VERSION 56) and later
            // These two methods return the internal FileMaker session ID and file ID that the expression environment is operating on.
            // It can be used to keep track of multiple FileMaker users/clients that are performing tasks in the same process on
            // different threads. A value of zero means that there isn't one of those objects available in the current environment.
            // Note: these numbers are not unique, the IDs may be reused after a session or file is shutdown. Check out the new
            // entry point messages that indicate when sessions and files are shutdown.
            inline ptrtype SessionID() const;
            inline ptrtype FileID() const;

            // Dynamic Registration of Plug-in Functions
            //
            // RegisterExternalFunction enables the plug-in to register a function with the application,
            // so that function appears in the calculation dialog in the application.
            //
            // "pluginId" should be the unique four-digit identifier for your plug-in that you use for the "options" string.
            // "functionId" is the unique id that you can use to represent which function was called, it will be passed back to the
            //    registered function as the first parameter (see the parameter of the same name in "ExtPluginType").
            // "functionName" is the name of the function as it should appear in the calculation formula.
            // "functionPrototype" is the suggested syntax that will appear in the list of functions in the calculation dialog.
            // "minArgs" is the number of required parameters for the function.  0 is the smallest valid value.
            // "maxArgs" is the maximum number of parameters that they user should be able to specify in the calculation dialog
            //     and still have correct syntax usage for the function.  Use -1 to allow a variable number of parameters up to the
            //     number supported by calculation formulas in the application.
            // "compatibleOnFlags" see bit flags above.
            // "funcPtr" is the pointer to the function that must match the signature defined by ExtPluginType.
            //
            //
            // UnRegisterExternalFunction removes the function entry from the list of external functions in any calculation dialog.
            //
            inline static errcode RegisterExternalFunction ( const QuadChar &pluginId, short functionId, const Text &functionName,
                                                                const Text &functionPrototype, short minArgs, short maxArgs, uint32 compatibleOnFlags, ExtPluginType funcPtr );
            inline static errcode UnRegisterExternalFunction ( const QuadChar &pluginId, short functionId );

            // New to FileMaker Pro 15 (API VERSION 56) and later
            // Same as RegisterExternalFunction except with additional parameter for the define calculation dialog
            //
            // "functionDescription" format is "type ahead word list|description text"
            //
            // If this version is not used to register the function, the description is generated by setting the "type ahead word list" to the function name with all
            // underscore characters converted to spaces and leaving the "description text" blank (i.e. BE_FileType_All becomes "BE FileType All|")
            inline static errcode RegisterExternalFunctionEx ( const QuadChar &pluginId, short functionId, const Text &functionName, const Text &functionPrototype,
                                                               const Text &functionDescription, short minArgs, short maxArgs, uint32 compatibleOnFlags, ExtPluginType funcPtr );

            // New to FileMaker Pro 16 (API VERSION 57) and later
            // Dynamic Registration of Script Steps
            //
            // RegisterScriptStep enables the plug-in to register a script step with the application,
            // so that script step appears in the scripting workspace in the application.
            //
            // "pluginId" should be the unique four-digit identifier for your plug-in that you use for the "options" string.
            // "scriptStepId" is the unique id that you can use to represent which script step was called, it will be passed back
            //    to the registered function as the first parameter (see the parameter of the same name in "ExtPluginType").
            // "scriptStepName" is the name of the script step as it should appear in the scripting workspace.
            // "scriptStepDefinition" is the XML definition of what parameters should be displayed for the script step.
            // "scriptStepDescription" is the description of the script step that is presented to the user when the step is selected.
            //    This uses the same syntax as the external function registration method.
            // "compatibleOnFlags" see bit flags above.
            // "funcPtr" is the pointer to the function that must match the signature defined by ExtPluginType.
            //
            // UnRegisterScriptStep removes the script step entry from the list of script steps in the scripting workspace.
            //
            inline static errcode RegisterScriptStep ( const QuadChar &pluginId, short scriptStepId, const Text &scriptStepName,
                                                       const Text &scriptStepDefinition, const Text &scriptStepDescription, uint32 compatibleOnFlags, ExtPluginType funcPtr );
            inline static errcode UnRegisterScriptStep ( const QuadChar &pluginId, short scriptStepId );

        private:
            ExprEnv ();
            ExprEnv ( const ExprEnv& other );

        };

        // DEPRECATED in FileMaker Pro 15. C++11 deprecated std::auto_ptr and replaced with std::unique_ptr.
        class DEPRECATED DataVectAutoPtr : public std::auto_ptr<DataVect>
        {
            typedef DataVectAutoPtr     UpCaster;
        public:
            inline DataVectAutoPtr ();
        };

        // DEPRECATED in FileMaker Pro 15. C++11 deprecated std::auto_ptr and replaced with std::unique_ptr.
        class DEPRECATED RowVectAutoPtr : public std::auto_ptr<RowVect>
        {
            typedef RowVectAutoPtr      UpCaster;
        public:
            inline RowVectAutoPtr ();
        };

        // DEPRECATED in FileMaker Pro 15. C++11 deprecated std::auto_ptr and replaced with std::unique_ptr.
        class DEPRECATED ExprEnvAutoPtr : public std::auto_ptr<ExprEnv>
        {
            typedef ExprEnvAutoPtr      UpCaster;
        public:
            inline ExprEnvAutoPtr ();
        };
        
#if FMX_USE_UNIQUE_PTR
        class DataVectUniquePtr : public std::unique_ptr<DataVect>
        {
            typedef DataVectUniquePtr     UpCaster;
        public:
            inline DataVectUniquePtr ();
        };
        
        class RowVectUniquePtr : public std::unique_ptr<RowVect>
        {
            typedef RowVectUniquePtr      UpCaster;
        public:
            inline RowVectUniquePtr ();
        };
        
        class ExprEnvUniquePtr : public std::unique_ptr<ExprEnv>
        {
            typedef ExprEnvUniquePtr      UpCaster;
        public:
            inline ExprEnvUniquePtr ();
        };
#endif
    }
}


// These functions are only useful for C-only environments or for porting to other languages (ex., Pascal).
extern "C"
{

    fmx::DataVect FMX_API *FM_DataVect_Constructor1 ( fmx::_fmxcpt &_x ) throw ();
    fmx::uint32 FMX_API FM_DataVect_Size ( const void *_self, fmx::_fmxcpt &_x ) throw ();
    void FMX_API FM_DataVect_Clear ( void *_self, fmx::_fmxcpt &_x ) throw ();
    bool FMX_API FM_DataVect_IsEmpty ( const void *_self, fmx::_fmxcpt &_x ) throw ();
    void FMX_API FM_DataVect_PushBack ( void *_self, const fmx::Data &data, fmx::_fmxcpt &_x ) throw ();
    fmx::Data FMX_API *FM_DataVect_PopBack ( void *_self, fmx::_fmxcpt &_x ) throw ();
    const fmx::Data FMX_API *FM_DataVect_At ( const void *_self, fmx::uint32 position, fmx::_fmxcpt &_x ) throw ();
    const fmx::Text FMX_API *FM_DataVect_AtAsText ( const void *_self, fmx::uint32 position, fmx::_fmxcpt &_x ) throw ();
    const fmx::FixPt FMX_API *FM_DataVect_AtAsNumber ( const void *_self, fmx::uint32 position, fmx::_fmxcpt &_x ) throw ();
    const fmx::DateTime FMX_API *FM_DataVect_AtAsDate ( const void *_self, fmx::uint32 position, fmx::_fmxcpt &_x ) throw ();
    const fmx::DateTime FMX_API *FM_DataVect_AtAsTime ( const void *_self, fmx::uint32 position, fmx::_fmxcpt &_x ) throw ();
    const fmx::DateTime FMX_API *FM_DataVect_AtAsTimeStamp ( const void *_self, fmx::uint32 position, fmx::_fmxcpt &_x ) throw ();
    bool FMX_API FM_DataVect_AtAsBoolean ( const void *_self, fmx::uint32 position, fmx::_fmxcpt &_x ) throw ();
    const fmx::BinaryData FMX_API *FM_DataVect_AtAsBinaryData ( const void *_self, fmx::uint32 position, fmx::_fmxcpt &_x ) throw ();
    void FMX_API FM_DataVect_Delete ( void *_self, fmx::_fmxcpt &_x ) throw ();

    fmx::RowVect FMX_API *FM_RowVect_Constructor1 ( fmx::_fmxcpt &_x ) throw ();
    fmx::uint32 FMX_API FM_RowVect_Size ( const void *_self, fmx::_fmxcpt &_x ) throw ();
    bool FMX_API FM_RowVect_IsEmpty ( const void *_self, fmx::_fmxcpt &_x ) throw ();
    const fmx::DataVect FMX_API *FM_RowVect_At ( const void *_self, fmx::uint32 position, fmx::_fmxcpt &_x ) throw ();
    void FMX_API FM_RowVect_Delete ( void *_self, fmx::_fmxcpt &_x ) throw ();

    fmx::ExprEnv FMX_API *FM_ExprEnv_Constructor1 ( fmx::_fmxcpt &_x ) throw ();
    fmx::errcode FMX_API FM_ExprEnv_Evaluate ( const void *_self, const fmx::Text &expression, fmx::Data &result, fmx::_fmxcpt &_x ) throw ();
#if FMX_OBSOLETE_API
    DEPRECATED fmx::errcode FMX_API FM_ExprEnv_ExecuteSQL ( const void *_self, const fmx::Text &expression, fmx::Data &result, fmx::uint16 colSep, fmx::uint16 rowSep, fmx::_fmxcpt &_x ) throw ();
#endif  
    fmx::errcode FMX_API FM_ExprEnv_ExecuteFileSQL ( const void *_self, const fmx::Text &expression, const fmx::Text &filename, const fmx::DataVect &parameters, fmx::RowVect& result, fmx::_fmxcpt &_x ) throw ();
    fmx::errcode FMX_API FM_ExprEnv_ExecuteFileSQLTextResult ( const void *_self, const fmx::Text &expression, const fmx::Text &filename, const fmx::DataVect &parameters, fmx::Data &result, fmx::uint16 colSep, fmx::uint16 rowSep, fmx::_fmxcpt &_x ) throw ();
    fmx::errcode FMX_API FM_ExprEnv_RegisterExternalFunction ( const fmx::QuadChar &pluginId, short functionId, const fmx::Text &functionName, const fmx::Text &functionPrototype, short minArgs, short maxArgs, fmx::uint32 compatibleOnFlags, fmx::ExtPluginType funcPtr, fmx::_fmxcpt &_x ) throw ();
    fmx::errcode FMX_API FM_ExprEnv_RegisterExternalFunctionEx ( const fmx::QuadChar &pluginId, short functionId, const fmx::Text &functionName, const fmx::Text &functionPrototype, const fmx::Text &functionDescription, short minArgs, short maxArgs, fmx::uint32 compatibleOnFlags, fmx::ExtPluginType funcPtr, fmx::_fmxcpt &_x ) throw ();
    fmx::errcode FMX_API FM_ExprEnv_UnRegisterExternalFunction ( const fmx::QuadChar &pluginId, short functionId, fmx::_fmxcpt &_x ) throw ();
    fmx::ptrtype FMX_API FM_ExprEnv_SessionID( const void *_self, fmx::_fmxcpt &_x ) throw ();
    fmx::ptrtype FMX_API FM_ExprEnv_FileID( const void *_self, fmx::_fmxcpt &_x ) throw ();
    fmx::errcode FMX_API FM_ExprEnv_RegisterScriptStep ( const fmx::QuadChar &pluginId, short scriptStepId, const fmx::Text &scriptStepName, const fmx::Text &scriptStepDefinition, const fmx::Text &scriptStepDescription, fmx::uint32 compatibleOnFlags, fmx::ExtPluginType funcPtr, fmx::_fmxcpt &_x );
    fmx::errcode FMX_API FM_ExprEnv_UnRegisterScriptStep ( const fmx::QuadChar &pluginId, short scriptStepId, fmx::_fmxcpt &_x );
    void FMX_API FM_ExprEnv_Delete ( void *_self, fmx::_fmxcpt &_x ) throw ();
}


extern "C++"
{
    namespace fmx
    {
        inline uint32 DataVect::Size () const
        {
            _fmxcpt _x;
            uint32 _rtn = FM_DataVect_Size ( this, _x );
            _x.Check ();
            return _rtn;
        }
        inline void DataVect::Clear ()
        {
            _fmxcpt _x;
            FM_DataVect_Clear ( this, _x );
            _x.Check ();
        }
        inline bool DataVect::IsEmpty () const
        {
            _fmxcpt _x;
            bool _rtn = FM_DataVect_IsEmpty ( this, _x );
            _x.Check ();
            return _rtn;
        }
        inline void DataVect::PushBack ( const Data &data )
        {
            _fmxcpt _x;
            FM_DataVect_PushBack ( this, data, _x );
            _x.Check ();
        }
        inline Data *DataVect::PopBack ()
        {
            _fmxcpt _x;
            Data *_rtn = FM_DataVect_PopBack ( this, _x );
            _x.Check ();
            return _rtn;
        }
        inline const Data &DataVect::At ( uint32 position ) const
        {
            _fmxcpt _x;
            const Data *_rtn = FM_DataVect_At ( this, position, _x );
            _x.Check ();
            return *_rtn;
        }
        inline const Text &DataVect::AtAsText ( uint32 position ) const
        {
            _fmxcpt _x;
            const Text *_rtn = FM_DataVect_AtAsText ( this, position, _x );
            _x.Check ();
            return *_rtn;
        }
        inline const FixPt &DataVect::AtAsNumber ( uint32 position ) const
        {
            _fmxcpt _x;
            const FixPt *_rtn = FM_DataVect_AtAsNumber ( this, position, _x );
            _x.Check ();
            return *_rtn;
        }
        inline const DateTime &DataVect::AtAsDate ( uint32 position ) const
        {
            _fmxcpt _x;
            const DateTime *_rtn = FM_DataVect_AtAsDate ( this, position, _x );
            _x.Check ();
            return *_rtn;
        }
        inline const DateTime &DataVect::AtAsTime ( uint32 position ) const
        {
            _fmxcpt _x;
            const DateTime *_rtn = FM_DataVect_AtAsTime ( this, position, _x );
            _x.Check ();
            return *_rtn;
        }
        inline const DateTime &DataVect::AtAsTimeStamp ( uint32 position ) const
        {
            _fmxcpt _x;
            const DateTime *_rtn = FM_DataVect_AtAsTimeStamp ( this, position, _x );
            _x.Check ();
            return *_rtn;
        }
        inline bool DataVect::AtAsBoolean ( uint32 position ) const
        {
            _fmxcpt _x;
            bool _rtn = FM_DataVect_AtAsBoolean ( this, position, _x );
            _x.Check ();
            return _rtn;
        }
        inline const BinaryData &DataVect::AtAsBinaryData ( uint32 position ) const
        {
            _fmxcpt _x;
            const BinaryData *_rtn = FM_DataVect_AtAsBinaryData ( this, position, _x );
            _x.Check ();
            return *_rtn;
        }
        inline void DataVect::operator delete ( void *obj )
        {
            _fmxcpt _x;
            FM_DataVect_Delete ( obj, _x );
            _x.Check ();
        }


        inline DataVectAutoPtr::DataVectAutoPtr ()
        {
            _fmxcpt _x;
            reset ( FM_DataVect_Constructor1 ( _x ) );
            _x.Check ();
        }
        
#if FMX_USE_UNIQUE_PTR
        inline DataVectUniquePtr::DataVectUniquePtr ()
        {
            _fmxcpt _x;
            reset ( FM_DataVect_Constructor1 ( _x ) );
            _x.Check ();
        }
#endif

        inline uint32 RowVect::Size () const
        {
            _fmxcpt _x;
            uint32 _rtn = FM_RowVect_Size ( this, _x );
            _x.Check ();
            return _rtn;
        }
        inline bool RowVect::IsEmpty () const
        {
            _fmxcpt _x;
            bool _rtn = FM_RowVect_IsEmpty ( this, _x );
            _x.Check ();
            return _rtn;
        }
        inline const DataVect &RowVect::At ( uint32 position ) const
        {
            _fmxcpt _x;
            const DataVect *_rtn = FM_RowVect_At ( this, position, _x );
            _x.Check ();
            return *_rtn;
        }
        inline void RowVect::operator delete ( void *obj )
        {
            _fmxcpt _x;
            FM_RowVect_Delete ( obj, _x );
            _x.Check ();
        }

        inline RowVectAutoPtr::RowVectAutoPtr ()
        {
            _fmxcpt _x;
            reset ( FM_RowVect_Constructor1 ( _x ) );
            _x.Check ();
        }

#if FMX_USE_UNIQUE_PTR
        inline RowVectUniquePtr::RowVectUniquePtr ()
        {
            _fmxcpt _x;
            reset ( FM_RowVect_Constructor1 ( _x ) );
            _x.Check ();
        }
#endif

        inline errcode ExprEnv::Evaluate ( const Text &expression, Data &result ) const
        {
            _fmxcpt _x;
            errcode _rtn = FM_ExprEnv_Evaluate ( this, expression, result, _x );
            _x.Check ();
            return _rtn;
        }
        inline errcode ExprEnv::ExecuteFileSQL ( const Text &expression, const Text &filename, const DataVect &parameters, RowVect& result ) const
        {
            _fmxcpt _x;
            errcode _rtn = FM_ExprEnv_ExecuteFileSQL ( this, expression, filename, parameters, result, _x );
            _x.Check ();
            return _rtn;
        }
        inline errcode ExprEnv::ExecuteFileSQLTextResult ( const Text &expression, const Text &filename, const DataVect &parameters, Data &result, uint16 colSep, uint16 rowSep ) const
        {
            _fmxcpt _x;
            errcode _rtn = FM_ExprEnv_ExecuteFileSQLTextResult ( this, expression, filename, parameters, result, colSep, rowSep, _x );
            _x.Check ();
            return _rtn;
        }
        inline errcode ExprEnv::RegisterExternalFunction ( const QuadChar &pluginId, short functionId, const Text &functionName, const Text &functionPrototype, short minArgs, short maxArgs, uint32 compatibleOnFlags, ExtPluginType funcPtr )
        {
            _fmxcpt _x;
            errcode _rtn = FM_ExprEnv_RegisterExternalFunction ( pluginId, functionId, functionName, functionPrototype, minArgs, maxArgs, compatibleOnFlags, funcPtr, _x );
            _x.Check ();
            return _rtn;
        }
        inline errcode ExprEnv::RegisterExternalFunctionEx ( const QuadChar &pluginId, short functionId, const Text &functionName, const Text &functionPrototype, const Text &functionDescription, short minArgs, short maxArgs, uint32 compatibleOnFlags, ExtPluginType funcPtr )
        {
            _fmxcpt _x;
            errcode _rtn = FM_ExprEnv_RegisterExternalFunctionEx ( pluginId, functionId, functionName, functionPrototype, functionDescription, minArgs, maxArgs, compatibleOnFlags, funcPtr, _x );
            _x.Check ();
            return _rtn;
        }
        inline errcode ExprEnv::UnRegisterExternalFunction ( const QuadChar &pluginId, short functionId )
        {
            _fmxcpt _x;
            errcode _rtn = FM_ExprEnv_UnRegisterExternalFunction ( pluginId, functionId, _x );
            _x.Check ();
            return _rtn;
        }
        inline ptrtype ExprEnv::SessionID () const
        {
            _fmxcpt _x;
            ptrtype _rtn = FM_ExprEnv_SessionID ( this, _x );
            _x.Check ();
            return _rtn;
        }
        inline ptrtype ExprEnv::FileID () const
        {
            _fmxcpt _x;
            ptrtype _rtn = FM_ExprEnv_FileID ( this, _x );
            _x.Check ();
            return _rtn;
        }
        inline errcode ExprEnv::RegisterScriptStep ( const QuadChar &pluginId, short scriptStepId, const Text &scriptStepName, const Text &scriptStepDefinition, const Text &scriptStepDescription, uint32 compatibleOnFlags, ExtPluginType funcPtr )
        {
            _fmxcpt _x;
            errcode _rtn = FM_ExprEnv_RegisterScriptStep ( pluginId, scriptStepId, scriptStepName, scriptStepDefinition, scriptStepDescription, compatibleOnFlags, funcPtr, _x );
            _x.Check ();
            return _rtn;
        }
        inline errcode ExprEnv::UnRegisterScriptStep ( const QuadChar &pluginId, short scriptStepId )
        {
            _fmxcpt _x;
            errcode _rtn = FM_ExprEnv_UnRegisterScriptStep ( pluginId, scriptStepId, _x );
            _x.Check ();
            return _rtn;
        }
        inline void ExprEnv::operator delete ( void *obj )
        {
            _fmxcpt _x;
            FM_ExprEnv_Delete ( obj, _x );
            _x.Check ();
        }

        inline ExprEnvAutoPtr::ExprEnvAutoPtr ()
        {
            _fmxcpt _x;
            reset ( FM_ExprEnv_Constructor1 ( _x ) );
            _x.Check ();
        }
        
#if FMX_USE_UNIQUE_PTR
        inline ExprEnvUniquePtr::ExprEnvUniquePtr ()
        {
            _fmxcpt _x;
            reset ( FM_ExprEnv_Constructor1 ( _x ) );
            _x.Check ();
        }
#endif

    }
}


#endif /* _h_DBCalcEngine_ */
