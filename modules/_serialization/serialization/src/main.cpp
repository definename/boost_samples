#include "pch.h"
#include "DarthVader.h"
#include "LukeSkywalker.h"
#include "Test.h"

//! Serializes.
template <typename TOArch, typename TSource, typename TTarget>
void SerializeTo(const TSource& source, TTarget& target);
//! De serializes.
template <typename TIArch, typename TSource, typename TTarget>
void DeserializeFrom(TSource& source, TTarget& target);

int main()
{
	try
	{
		{
			std::ofstream ofile("test.dat", std::ios::out);
			Test test(999, true);
			SerializeTo<boost::archive::text_oarchive>(test, ofile);

			std::ifstream ifile("test.dat", std::ios::in);
			DeserializeFrom<boost::archive::text_iarchive>(ifile, test);
			std::cout << test.GetData() << std::endl;
		}

		{
			std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);
			{
				boost::shared_ptr<Character> luke = boost::make_shared<LukeSkywalker>("Luke Skywalker", 99);
				boost::shared_ptr<Character> darth = boost::make_shared<DarthVader>("Darth Vader", 60);
				std::vector<boost::shared_ptr<Character>> wars = { luke, darth };
				SerializeTo<boost::archive::binary_oarchive>(wars, stream);
			}

			{
				std::vector<boost::shared_ptr<Character>> wars;
				DeserializeFrom <boost::archive::binary_iarchive>(stream, wars);

				for (const auto& val : wars)
				{
					std::cout << "Power: " << val->GetPower() << std::endl;
					std::cout << "Name: " << val->GetName() << std::endl;
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error occurred: " << e.what();
	}

	return 0;
}

template <typename TOArch, typename TSource, typename TTarget>
void SerializeTo(const TSource& source, TTarget& target) {
	TOArch oa(target);
	oa << source;
}

template <typename TIArch, typename TSource, typename TTarget>
void DeserializeFrom(TSource& source, TTarget& target) {
	TIArch ia(source);
	ia >> target;
}