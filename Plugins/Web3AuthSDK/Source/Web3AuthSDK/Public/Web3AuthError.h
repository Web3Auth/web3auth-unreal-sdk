#pragma once

#include "CoreMinimal.h"
#include <string>

enum ErrorCode
{
	NOUSERFOUND,
	ENCODING_ERROR,
	DECODING_ERROR,
	SOMETHING_WENT_WRONG,
	RUNTIME_ERROR,
	APP_CANCELLED
};

class Web3AuthError
{
public:
	static FString getError(ErrorCode code) {
		switch (code)
		{
		case ErrorCode::NOUSERFOUND:
			return "No user found, please login again!";
			break;
		case ErrorCode::ENCODING_ERROR:
			return "Encoding Error";
			break;
		case ErrorCode::DECODING_ERROR:
			return "Decoding Error";
			break;
		case ErrorCode::SOMETHING_WENT_WRONG:
			return "Something went wrong!";
			break;
		case ErrorCode::RUNTIME_ERROR:
			return "Runtime Error";
			break;
		case ErrorCode::APP_CANCELLED:
			return "App Cancelled";
			break;
		}
		return "";
	}
};