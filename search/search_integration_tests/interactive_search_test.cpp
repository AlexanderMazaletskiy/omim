#include "testing/testing.hpp"

#include "generator/generator_tests_support/test_feature.hpp"

#include "search/viewport_search_callback.hpp"
#include "search/mode.hpp"
#include "search/search_integration_tests/helpers.hpp"
#include "search/search_tests_support/test_results_matching.hpp"
#include "search/search_tests_support/test_search_request.hpp"

#include "base/macros.hpp"

using namespace generator::tests_support;
using namespace search::tests_support;

namespace search
{
namespace
{
class TestCafe : public TestPOI
{
public:
  TestCafe(m2::PointD const & center) : TestPOI(center, "cafe", "en")
  {
    SetTypes({{"amenity", "cafe"}});
  }
};

class TestHotel : public TestPOI
{
public:
  TestHotel(m2::PointD const & center) : TestPOI(center, "hotel", "en")
  {
    SetTypes({{"tourism", "hotel"}});
  }
};

class TestDelegate : public ViewportSearchCallback::Delegate
{
public:
  TestDelegate(bool & mode) : m_mode(mode) {}

  // ViewportSearchCallback::Delegate overrides:
  void RunUITask(function<void()> /* fn */) override {}
  void SetHotelDisplacementMode() override { m_mode = true; }
  bool IsViewportSearchActive() const override { return true; }
  void ShowViewportSearchResults(Results const & /* results */) override {}
  void ClearViewportSearchResults() override {}

 private:
  bool & m_mode;
};

class InteractiveSearchRequest : public TestDelegate, public TestSearchRequest
{
public:
  InteractiveSearchRequest(TestSearchEngine & engine, string const & query,
                           m2::RectD const & viewport, bool & mode)
    : TestDelegate(mode)
    , TestSearchRequest(engine, query, "en" /* locale */, Mode::Viewport, viewport)
  {
    SetCustomOnResults(
        ViewportSearchCallback(static_cast<ViewportSearchCallback::Delegate &>(*this),
                               bind(&InteractiveSearchRequest::OnResults, this, _1)));
  }
};

class InteractiveSearchTest : public SearchTest
{
};

double const kDX[] = {-0.01, 0, 0, 0.01};
double const kDY[] = {0, -0.01, 0.01, 0};

static_assert(ARRAY_SIZE(kDX) == ARRAY_SIZE(kDY), "Wrong deltas lists");

UNIT_CLASS_TEST(InteractiveSearchTest, Smoke)
{
  m2::PointD const cafesPivot(-1, -1);
  m2::PointD const hotelsPivot(1, 1);

  vector<TestCafe> cafes;
  for (size_t i = 0; i < ARRAY_SIZE(kDX); ++i)
    cafes.emplace_back(m2::Shift(cafesPivot, kDX[i], kDY[i]));

  vector<TestHotel> hotels;
  for (size_t i = 0; i < ARRAY_SIZE(kDX); ++i)
    hotels.emplace_back(m2::Shift(hotelsPivot, kDX[i], kDY[i]));

  auto const id = BuildCountry("Wonderland", [&](TestMwmBuilder & builder) {
    for (auto const & cafe : cafes)
      builder.Add(cafe);
    for (auto const & hotel : hotels)
      builder.Add(hotel);
  });

  {
    bool mode = false;
    InteractiveSearchRequest request(
        m_engine, "cafe", m2::RectD(m2::PointD(-1.5, -1.5), m2::PointD(-0.5, -0.5)), mode);
    request.Run();

    TRules const rules = {ExactMatch(id, cafes[0]), ExactMatch(id, cafes[1]),
                          ExactMatch(id, cafes[2]), ExactMatch(id, cafes[3])};

    TEST(!mode, ());
    TEST(MatchResults(m_engine, rules, request.Results()), ());
  }

  {
    bool mode = false;
    InteractiveSearchRequest request(m_engine, "hotel",
                                     m2::RectD(m2::PointD(0.5, 0.5), m2::PointD(1.5, 1.5)), mode);
    request.Run();

    TRules const rules = {ExactMatch(id, hotels[0]), ExactMatch(id, hotels[1]),
                          ExactMatch(id, hotels[2]), ExactMatch(id, hotels[3])};

    TEST(mode, ());
    TEST(MatchResults(m_engine, rules, request.Results()), ());
  }
}
}  // namespace
}  // namespace search
